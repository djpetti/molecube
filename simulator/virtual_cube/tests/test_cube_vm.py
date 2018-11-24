import mock
import os
import subprocess
import unittest

from simulator.virtual_cube import cube_vm, serial_com


class TestCubeVm(unittest.TestCase):
  """ Tests for the CubeVm class. """

  def setUp(self):
    # Reset the ID counter.
    cube_vm.CubeVm._CUBE_ID = 0
    # Create a CubeVm object for testing.
    self.__cube = cube_vm.CubeVm()

  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  def test_attach_to(self, mocked_serial):
    """ Tests that the attach_to argument behaves properly. """
    # Attach to a fake serial handle.
    cube = cube_vm.CubeVm(attach_to="cube")

    # It should have set the serial handle.
    self.assertEqual("/tmp/cube", cube.get_serial())
    # It should have created the SerialCom instance.
    mocked_serial.assert_called_once_with(cube.get_serial())

    # It should not allow us to start this cube manually.
    self.assertRaises(RuntimeError, cube.start)

  def test_get_serial(self):
    """ Tests that get_serial() works under normal conditions. """
    self.assertEqual("/tmp/cube0", self.__cube.get_serial())

    # A second one should have its own unique FD.
    cube = cube_vm.CubeVm()
    self.assertEqual("/tmp/cube1", cube.get_serial())

  def test_disk_image_extraction(self):
    """ Tests that it properly extracts the compressed disk image. """
    # If the disk image is already uncompressed, remove it.
    os.remove(cube_vm.CubeVm._DISK_IMAGE)

    # Create a new cube.
    cube = cube_vm.CubeVm()

    # It should have extracted it.
    self.assertTrue(os.path.exists(cube_vm.CubeVm._DISK_IMAGE))

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  def test_start(self, mocked_os, mocked_serial, mocked_popen):
    """ Tests that we can start the VM. """
    # Make it look like the serial handle exists.
    mocked_os.return_value = True

    self.__cube.start()

    # Make sure it started the process.
    expected_command = [cube_vm.CubeVm._QEMU_BIN, "-readconfig",
                        cube_vm.CubeVm._QEMU_CONFIG, "-nographic", "-chardev",
                        "socket,path=/tmp/cube0,server,nowait,id=vcube_ser"]
    mocked_popen.assert_called_once_with(expected_command,
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)

    # It should have checked for the serial handle.
    mocked_os.assert_called_once_with("/tmp/cube0")
    # It should have started the serial interface.
    self.__serial = mocked_serial.assert_called_once_with("/tmp/cube0")

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  def test_start_spin(self, mocked_os, mocked_serial, mocked_popen):
    """ Tests that we can start the VM when waiting for the serial takes
    multiple tries. """
    # Make it look like the serial handle doesn't exist initially and then does.
    mocked_os.side_effect = [False, True]

    self.__cube.start()

    # Make sure it started the process.
    expected_command = [cube_vm.CubeVm._QEMU_BIN, "-readconfig",
                        cube_vm.CubeVm._QEMU_CONFIG, "-nographic", "-chardev",
                        "socket,path=/tmp/cube0,server,nowait,id=vcube_ser"]
    mocked_popen.assert_called_once_with(expected_command,
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)

    # It should have checked for the serial handle twice.
    calls = [mock.call("/tmp/cube0")] * 2
    mocked_os.assert_has_calls(calls)
    # It should have started the serial interface.
    self.__serial = mocked_serial.assert_called_once_with("/tmp/cube0")

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  @mock.patch("os.path.exists")
  def test_stop(self, mocked_os, mocked_serial, mocked_popen):
    """ Tests that we can stop the VM. """
    # Make it look like the serial handle exists.
    mocked_os.return_value = True

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
