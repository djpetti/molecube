import mock
import os
import subprocess
import unittest

from config import config
from simulator.virtual_cube import cube_vm, serial_com


# Load the simulator and cube configurations.
sim_config = config.simulator_config()
cube_config = config.cube_config()


class TestCubeVm(unittest.TestCase):
  """ Tests for the CubeVm class. """

  @mock.patch("simulator.virtual_cube.cube_vm.CubeVm._copy_binaries")
  @mock.patch("os.path.exists")
  def setUp(self, mocked_exists, mocked_bin_copy):
    # Indicate that the binaries were copied, so it doesn't try to do that.
    cube_vm.CubeVm._COPIED_BINARIES = True
    # Make it look like the disk image exists so it doesn't go and try to
    # uncompress it.
    mocked_exists.return_value = True

    # Reset the ID counter.
    cube_vm.CubeVm._CUBE_ID = 0
    # Create a CubeVm object for testing.
    self.__cube = cube_vm.CubeVm()

  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  def test_attach_to(self, mocked_exists, mocked_serial):
    """ Tests that the attach_to argument behaves properly. """
    # Indicate that the binaries were copied, so it doesn't try to do that.
    cube_vm.CubeVm._COPIED_BINARIES = True
    # Make it look like the disk image exists so it doesn't go and try to
    # uncompress it.
    mocked_exists.return_value = True

    # Attach to a fake serial handle.
    cube = cube_vm.CubeVm(attach_to="cube")

    # It should have set the serial handle.
    self.assertEqual("/tmp/cube", cube.get_serial())
    # It should have created the SerialCom instance.
    mocked_serial.assert_called_once_with(cube.get_serial())

    # It should not allow us to start this cube manually.
    self.assertRaises(RuntimeError, cube.start)

  @mock.patch("os.path.exists")
  def test_get_serial(self, mocked_exists):
    """ Tests that get_serial() works under normal conditions. """
    self.assertEqual("/tmp/cube0", self.__cube.get_serial())

    # Make it look like the disk image exists so it doesn't go and try to
    # uncompress it.
    mocked_exists.return_value = True
    # Indicate that the binaries were copied, so it doesn't try to do that.
    cube_vm.CubeVm._COPIED_BINARIES = True

    # A second one should have its own unique FD.
    cube = cube_vm.CubeVm()
    self.assertEqual("/tmp/cube1", cube.get_serial())

  @mock.patch("os.path.exists")
  @mock.patch("gzip.open")
  @mock.patch("simulator.virtual_cube.cube_vm.open")
  def test_disk_image_extraction(self, mocked_open, mocked_gzip_open,
                                 mocked_exists):
    """ Tests that it properly extracts the compressed disk image. """
    # Make it look like the disk image does not exist.
    mocked_exists.return_value = False
    # Mock the compressed file.
    mock_uncompressed = mocked_open.return_value
    # Mock the GZIP context manager.
    mock_gzip_file = mocked_gzip_open.return_value.__enter__.return_value
    # Indicate that the binaries were copied, so it doesn't try to do that.
    cube_vm.CubeVm._COPIED_BINARIES = True

    # Create a new cube.
    cube = cube_vm.CubeVm()

    # It should have verified that it didn't already exist.
    disk_image = sim_config.get("qemu", "disk_image")
    compressed_path = disk_image + ".gz"
    mocked_exists.assert_called_once_with(disk_image)
    # It should have opened the GZIP file.
    mocked_open.assert_called_once_with(disk_image, "wb")
    mocked_gzip_open.assert_called_once_with(compressed_path, "rb")
    mock_gzip_file.read.assert_called_once_with()
    mock_uncompressed.write.assert_called_once()
    mock_uncompressed.close.assert_called_once_with()

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.path.isdir")
  def test_start(self, mocked_isdir, mocked_exists, mocked_serial,
                 mocked_popen):
    """ Tests that we can start the VM. """
    # Make it look like the serial handle doesn't exist before we create it, but
    # does exist afterwards.
    mocked_exists.side_effect = [False, True]
    # Make it look like the log directory exists.
    mocked_isdir.return_value = True

    self.__cube.start()

    # Make sure it started the process.
    qemu_bin = sim_config.get("qemu", "bin_location")
    qemu_config = sim_config.get("qemu", "config_location")
    expected_command = [qemu_bin, "-readconfig",
                        qemu_config, "-nographic", "-chardev",
                        "socket,path=/tmp/cube0,server,nowait,id=vcube_ser"]
    mocked_popen.assert_called_once_with(expected_command,
                                         stdout=mock.ANY)

    # It should have checked twice for the serial handle.
    expected_calls = [mock.call("/tmp/cube0")] * 2
    mocked_exists.assert_has_calls(expected_calls)
    # It should have checked for the log directory.
    log_dir = cube_config.get("logging", "host_log_dir")
    mocked_isdir.assert_called_once_with(log_dir)
    # It should have started the serial interface.
    self.__serial = mocked_serial.assert_called_once_with("/tmp/cube0")

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.path.isdir")
  def test_start_spin(self, mocked_isdir, mocked_exists, mocked_serial,
                      mocked_popen):
    """ Tests that we can start the VM when waiting for the serial takes
    multiple tries. """
    # Make it look like the serial handle doesn't exist before we create it, and
    # then takes some time to appear after we do.
    mocked_exists.side_effect = [False, False, True]
    # Make it look like the log directory exists.
    mocked_isdir.return_value = True

    self.__cube.start()

    # Make sure it started the process.
    qemu_bin = sim_config.get("qemu", "bin_location")
    qemu_config = sim_config.get("qemu", "config_location")
    expected_command = [qemu_bin, "-readconfig",
                        qemu_config, "-nographic", "-chardev",
                        "socket,path=/tmp/cube0,server,nowait,id=vcube_ser"]
    mocked_popen.assert_called_once_with(expected_command,
                                         stdout=mock.ANY)

    # It should have checked for the serial handle twice.
    calls = [mock.call("/tmp/cube0")] * 3
    mocked_exists.assert_has_calls(calls)
    # It should have checked for the log directory.
    log_dir = cube_config.get("logging", "host_log_dir")
    mocked_isdir.assert_called_once_with(log_dir)
    # It should have started the serial interface.
    self.__serial = mocked_serial.assert_called_once_with("/tmp/cube0")

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("os.path.isdir")
  def test_start_has_old_handle(self, mocked_isdir, mocked_remove,
                                mocked_exists, mocked_serial, mocked_popen):
    """ Tests that we can start the VM when we have an old serial handle sitting
    around. """
    # Make it look like there is an old serial handle that is there before we
    # create it. Obviously, it should also exist afterwards.
    mocked_exists.side_effect = [True, True]
    # Make it look like the log directory exists.
    mocked_isdir.return_value = True

    self.__cube.start()

    # It should have deleted the old handle.
    mocked_remove.assert_called_once_with("/tmp/cube0")

    # It should have checked twice for the serial handle.
    expected_calls = [mock.call("/tmp/cube0")] * 2
    mocked_exists.assert_has_calls(expected_calls)

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("os.path.isdir")
  def test_start_no_old_handle(self, mocked_isdir, mocked_remove, mocked_exists,
                               mocked_serial, mocked_popen):
    """ Tests that we can start the VM when there is no old serial handle
    sitting around. """
    # Make it look like there is no old serial handle.
    mocked_exists.side_effect = [False, True]
    # Make it look like the log directory exists.
    mocked_isdir.return_value = True

    self.__cube.start()

    # It should not have deleted the old handle.
    mocked_remove.assert_not_called()

    # It should have checked twice for the serial handle.
    expected_calls = [mock.call("/tmp/cube0")] * 2
    mocked_exists.assert_has_calls(expected_calls)

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.path.isdir")
  @mock.patch("os.mkdir")
  def test_start_no_log_dir(self, mocked_mkdir, mocked_isdir, mocked_exists,
                            mocked_serial, mocked_popen):
    """ Tests that we can start the VM when the log directory needs to be
    created. """
    # Make it look like there is no old serial handle that is there before we
    # create it. It will exist after it is created.
    mocked_exists.side_effect = [False, True]
    # Make it look like the log directory does not exist.
    mocked_isdir.return_value = False

    self.__cube.start()

    # It should have checked for the log directory.
    log_dir = cube_config.get("logging", "host_log_dir")
    mocked_isdir.assert_called_once_with(log_dir)
    # It should have created it.
    mocked_mkdir.assert_called_once_with(log_dir)

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  @mock.patch("os.path.isdir")
  @mock.patch("os.mkdir")
  def test_start_has_log_dir(self, mocked_mkdir, mocked_isdir, mocked_exists,
                             mocked_serial, mocked_popen):
    """ Tests that we can start the VM when the log directory already exists. """
    # Make it look like there is no old serial handle that is there before we
    # create it. It will exist after it is created.
    mocked_exists.side_effect = [False, True]
    # Make it look like the log directory exists.
    mocked_isdir.return_value = True

    self.__cube.start()

    # It should have checked for the log directory.
    log_dir = cube_config.get("logging", "host_log_dir")
    mocked_isdir.assert_called_once_with(log_dir)
    # It should not have created it.
    mocked_mkdir.assert_not_called()

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  def test_stop(self, mocked_exists, mocked_serial, mocked_popen):
    """ Tests that we can stop the VM. """
    # Make it look like the serial handle doesn't exist before we create it, but
    # does afterwards.
    mocked_exists.side_effect = [False, True]

    # Make popen return a dummy process.
    fake_process = mocked_popen.return_value

    # Run start() first to set our process.
    self.__cube.start()
    # Make sure Popen was called.
    mocked_popen.assert_called_once()
    # Make sure the serial interface was started.
    mocked_serial.assert_called_once()

    # Run stop.
    self.__cube.stop()
    # It should have waited for the process to finish.
    fake_process.wait.assert_called_once()

    # If we run stop again, it should do nothing.
    self.__cube.stop()
    fake_process.wait.assert_called_once()

  @mock.patch("os.path.isdir")
  @mock.patch("os.path.realpath")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("shutil.copy2")
  @mock.patch("os.mkdir")
  @mock.patch("shutil.copytree")
  @mock.patch("shutil.rmtree")
  def test_copy_binaries(self, mocked_rmtree, mocked_copytree, mocked_mkdir,
                         mocked_copy2, mocked_remove, mocked_exists,
                         mocked_realpath, mocked_isdir):
    """ Tests that _copy_binaries works under normal conditions. """
    cube_binaries = cube_config.get("binaries", "start_list")
    starter_path = cube_config.get("binaries", "starter_script")
    config_path = cube_config.get("binaries", "config_package")
    all_files = cube_binaries
    all_files.append(starter_path)
    all_files.append(config_path)

    # Make it look like the binary directory exists, but that the things it
    # needs to copy are not directories.
    mocked_isdir.side_effect = [True] + [False] * len(all_files)
    # For the sake of simplicity, we're just going to use the symlink paths as
    # the full paths.
    mocked_realpath.side_effect = all_files
    # Make it look like there are no old binaries.
    mocked_exists.return_value = False

    cube_vm.CubeVm._copy_binaries()

    # Make sure it checked for the binary directory, and whether the targets are
    # directories.
    bin_dir = cube_config.get("binaries", "host_bin_dir")
    expected_calls = [mock.call(bin_dir)] + \
                     [mock.call(path) for path in all_files]
    mocked_isdir.assert_has_calls(expected_calls)
    # It should not have tried to create it.
    mocked_mkdir.assert_not_called()

    # Make sure it got the un-symlinked path for all the executables.
    expected_calls = [mock.call(path) for path in all_files]
    mocked_realpath.assert_has_calls(expected_calls)

    # It should have checked for existing copies, and performed the copy
    # operation.
    self.assertEqual(len(all_files), mocked_exists.call_count)
    dest_calls = mocked_exists.call_args_list
    # The first index gets the args tuple, the second gets the first argument.
    dest_paths = [call[0][0] for call in dest_calls]
    copy_calls = [mock.call(src[1][0], dst) for src, dst in \
                  zip(expected_calls, dest_paths)]
    mocked_copy2.assert_has_calls(copy_calls)

    # Check that the destination paths are reasonable.
    for expected_path, actual_path in zip(all_files, dest_paths):
      head, tail = os.path.split(actual_path)

      # The tail component should be name of the binary.
      self.assertEqual(os.path.basename(expected_path), tail)
      # The head component should be the binary directory.
      self.assertEqual(bin_dir, head)

    # It should not have removed anything.
    mocked_remove.assert_not_called()
    mocked_rmtree.assert_not_called()
    # It should not have tried to copy directories.
    mocked_copytree.assert_not_called()

  @mock.patch("os.path.isdir")
  @mock.patch("os.path.realpath")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("shutil.copy2")
  @mock.patch("os.mkdir")
  @mock.patch("shutil.copytree")
  @mock.patch("shutil.rmtree")
  def test_copy_binaries_with_directories(self, mocked_rmtree, mocked_copytree,
                                          mocked_mkdir, mocked_copy2,
                                          mocked_remove, mocked_exists,
                                          mocked_realpath, mocked_isdir):
    """ Tests that _copy_binaries works when we have to copy directories too.
    """
    cube_binaries = cube_config.get("binaries", "start_list")
    starter_path = cube_config.get("binaries", "starter_script")
    config_path = cube_config.get("binaries", "config_package")
    all_files = cube_binaries
    all_files.append(starter_path)
    all_files.append(config_path)

    # Make it look like the binary directory exists, and that the things it
    # needs to copy are directories.
    mocked_isdir.side_effect = [True] + [True] * len(all_files)
    # For the sake of simplicity, we're just going to use the symlink paths as
    # the full paths.
    mocked_realpath.side_effect = all_files
    # Make it look like there are no old binaries.
    mocked_exists.return_value = False

    cube_vm.CubeVm._copy_binaries()

    # Make sure it checked for the binary directory, and whether the targets are
    # directories.
    bin_dir = cube_config.get("binaries", "host_bin_dir")
    expected_calls = [mock.call(bin_dir)] + \
                     [mock.call(path) for path in all_files]
    mocked_isdir.assert_has_calls(expected_calls)
    # It should not have tried to create it.
    mocked_mkdir.assert_not_called()

    # Make sure it got the un-symlinked path for all the executables.
    expected_calls = [mock.call(path) for path in all_files]
    mocked_realpath.assert_has_calls(expected_calls)

    # It should have checked for existing copies, and performed the copy
    # operation.
    self.assertEqual(len(all_files), mocked_exists.call_count)
    dest_calls = mocked_exists.call_args_list
    # The first index gets the args tuple, the second gets the first argument.
    dest_paths = [call[0][0] for call in dest_calls]
    copy_calls = [mock.call(src[1][0], dst) for src, dst in \
                  zip(expected_calls, dest_paths)]
    mocked_copytree.assert_has_calls(copy_calls)

    # Check that the destination paths are reasonable.
    for expected_path, actual_path in zip(all_files, dest_paths):
      head, tail = os.path.split(actual_path)

      # The tail component should be name of the binary.
      self.assertEqual(os.path.basename(expected_path), tail)
      # The head component should be the binary directory.
      self.assertEqual(bin_dir, head)

    # It should not have removed anything.
    mocked_remove.assert_not_called()
    mocked_rmtree.assert_not_called()
    # It should not have tried to copy single files.
    mocked_copy2.assert_not_called()

  @mock.patch("os.path.isdir")
  @mock.patch("os.path.realpath")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("shutil.copy2")
  @mock.patch("os.mkdir")
  @mock.patch("shutil.copytree")
  @mock.patch("shutil.rmtree")
  def test_copy_binaries_no_bin_dir(self, mocked_rmtree, mocked_copytree,
                                    mocked_mkdir, mocked_copy2, mocked_remove,
                                    mocked_exists, mocked_realpath,
                                    mocked_isdir):
    """ Tests that _copy_binaries works when the target directory does not
    exist. """
    cube_binaries = cube_config.get("binaries", "start_list")
    starter_path = cube_config.get("binaries", "starter_script")
    config_path = cube_config.get("binaries", "config_package")
    all_files = cube_binaries
    all_files.append(starter_path)
    all_files.append(config_path)

    # Make it look like the binary directory doesn't exist, and the things it
    # needs to copy are not directories.
    mocked_isdir.side_effect = [False] + [False] * len(all_files)
    # For the sake of simplicity, we're just going to use the symlink paths as
    # the full paths.
    mocked_realpath.side_effect = all_files
    # Make it look like there are no old binaries.
    mocked_exists.return_value = False

    cube_vm.CubeVm._copy_binaries()

    # Make sure it checked for the binary directory, and whether the targets are
    # directories.
    bin_dir = cube_config.get("binaries", "host_bin_dir")
    expected_calls = [mock.call(bin_dir)] + \
                     [mock.call(path) for path in all_files]
    mocked_isdir.assert_has_calls(expected_calls)
    # Make sure it created it.
    mocked_mkdir.assert_called_once_with(bin_dir)

    # Make sure that all the per-binary calls were made.
    self.assertEqual(len(all_files), mocked_realpath.call_count)
    self.assertEqual(len(all_files), mocked_exists.call_count)
    self.assertEqual(len(all_files), mocked_copy2.call_count)

    # It should not have removed anything.
    mocked_remove.assert_not_called()
    mocked_rmtree.assert_not_called()
    # It should not have tried to copy directories.
    mocked_copytree.assert_not_called()

  @mock.patch("os.path.isdir")
  @mock.patch("os.path.realpath")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("shutil.copy2")
  @mock.patch("os.mkdir")
  @mock.patch("shutil.copytree")
  @mock.patch("shutil.rmtree")
  def test_copy_binaries_old_copy(self, mocked_rmtree, mocked_copytree,
                                  mocked_mkdir, mocked_copy2, mocked_remove,
                                  mocked_exists, mocked_realpath, mocked_isdir):
    """ Tests that _copy_binaries works when there are old copies of the
    binaries. """
    cube_binaries = cube_config.get("binaries", "start_list")
    starter_path = cube_config.get("binaries", "starter_script")
    config_path = cube_config.get("binaries", "config_package")
    all_files = cube_binaries
    all_files.append(starter_path)
    all_files.append(config_path)

    # Make it look like the binary directory exists, but that the things it
    # needs to copy are not directories. Also, make it look like the existing
    # items in the binary directory are not directories.
    mocked_isdir.side_effect = [True] + [False] * len(all_files) * 2
    # For the sake of simplicity, we're just going to use the symlink paths as
    # the full paths.
    mocked_realpath.side_effect = all_files
    # Make it look like there are old binaries.
    mocked_exists.return_value = True

    cube_vm.CubeVm._copy_binaries()

    # Compute expected destination paths.
    bin_dir = cube_config.get("binaries", "host_bin_dir")
    dest_paths = []
    for path in all_files:
      dest_path = os.path.join(bin_dir, os.path.basename(path))
      dest_paths.append(dest_path)

    # Make sure it checked for the binary directory, and whether the targets are
    # directories.
    # Initial check for the binary directory.
    expected_calls = [mock.call(bin_dir)]
    for src_path, dst_path in zip(all_files, dest_paths):
      # It will test the existing old binary first.
      expected_calls.append(mock.call(dst_path))
      # Then it will test the source binary.
      expected_calls.append(mock.call(src_path))
    mocked_isdir.assert_has_calls(expected_calls)
    # It should not have tried to create it.
    mocked_mkdir.assert_not_called()

    # Make sure that all the per-binary calls were made.
    self.assertEqual(len(all_files), mocked_realpath.call_count)
    self.assertEqual(len(all_files), mocked_exists.call_count)
    self.assertEqual(len(all_files), mocked_copy2.call_count)

    # Check that it removed the old binaries.
    expected_calls = [mock.call(path) for path in dest_paths]
    mocked_remove.assert_has_calls(expected_calls)

    # It should not have removed directories.
    mocked_rmtree.assert_not_called()
    # It should not have tried to copy directories.
    mocked_copytree.assert_not_called()

  @mock.patch("os.path.isdir")
  @mock.patch("os.path.realpath")
  @mock.patch("os.path.exists")
  @mock.patch("os.remove")
  @mock.patch("shutil.copy2")
  @mock.patch("os.mkdir")
  @mock.patch("shutil.copytree")
  @mock.patch("shutil.rmtree")
  def test_copy_binaries_old_copy_dir(self, mocked_rmtree, mocked_copytree,
                                      mocked_mkdir, mocked_copy2, mocked_remove,
                                      mocked_exists, mocked_realpath,
                                      mocked_isdir):
    """ Tests that _copy_binaries works when there are old copies of the
    binaries, including old directories. """
    cube_binaries = cube_config.get("binaries", "start_list")
    starter_path = cube_config.get("binaries", "starter_script")
    config_path = cube_config.get("binaries", "config_package")
    all_files = cube_binaries
    all_files.append(starter_path)
    all_files.append(config_path)

    # Make it look like the binary directory exists, but that the things it
    # needs to copy are not directories. Also, make it look like the existing
    # items in the binary directory are directories.
    mocked_isdir.side_effect = [True] + [True, False] * len(all_files)
    # For the sake of simplicity, we're just going to use the symlink paths as
    # the full paths.
    mocked_realpath.side_effect = all_files
    # Make it look like there are old binaries.
    mocked_exists.return_value = True

    cube_vm.CubeVm._copy_binaries()

    # Compute expected destination paths.
    bin_dir = cube_config.get("binaries", "host_bin_dir")
    dest_paths = []
    for path in all_files:
      dest_path = os.path.join(bin_dir, os.path.basename(path))
      dest_paths.append(dest_path)

    # Make sure it checked for the binary directory, and whether the targets are
    # directories.
    # Initial check for the binary directory.
    expected_calls = [mock.call(bin_dir)]
    for src_path, dst_path in zip(all_files, dest_paths):
      # It will test the existing old binary first.
      expected_calls.append(mock.call(dst_path))
      # Then it will test the source binary.
      expected_calls.append(mock.call(src_path))
    mocked_isdir.assert_has_calls(expected_calls)
    # It should not have tried to create it.
    mocked_mkdir.assert_not_called()

    # Make sure that all the per-binary calls were made.
    self.assertEqual(len(all_files), mocked_realpath.call_count)
    self.assertEqual(len(all_files), mocked_exists.call_count)
    self.assertEqual(len(all_files), mocked_copy2.call_count)

    # Check that it removed the old binaries.
    expected_calls = [mock.call(path) for path in dest_paths]
    mocked_rmtree.assert_has_calls(expected_calls)

    # It should not have removed single files.
    mocked_remove.assert_not_called()
    # It should not have tried to copy directories.
    mocked_copytree.assert_not_called()

  @mock.patch("simulator.virtual_cube.serial_com.SerialCom.select_on")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  def test_select_on(self, mocked_serial, mocked_select):
    """ Tests that select_on() works under normal conditions. """
    # We're just going to create some fake "SerialComs" so we can tell them
    # appart.
    fake_serials = []
    for i in range(0, 3):
      fake_serials.append(mock.MagicMock())
    mocked_serial.side_effect = fake_serials

    # Make some cubes to test with.
    cubes = []
    for i in range(0, 3):
      # We use attach_to here so we'll have valid serial interfaces without
      # actually starting the cubes.
      cubes.append(cube_vm.CubeVm(attach_to="cube%d" % (i)))

    # It should have initialized the SerialComs.
    expected_calls = []
    for cube in cubes:
      expected_calls.append(mock.call(cube.get_serial()))
    mocked_serial.assert_has_calls(expected_calls)

    # Make it look like the first one is readable.
    mocked_select.return_value = [fake_serials[0]]

    readable = cube_vm.CubeVm.select_on(cubes)

    # It should have indicated that the correct one is readable.
    self.assertListEqual([cubes[0]], readable)
    # It should have called select_on().
    mocked_select.assert_called_once()


if __name__ == "__main__":
  unittest.main()
