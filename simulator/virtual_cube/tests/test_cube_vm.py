import mock
import os
import subprocess
import unittest

from simulator.virtual_cube import cube_vm


class TestCubeVm(unittest.TestCase):
  """ Tests for the CubeVm class. """

  def setUp(self):
    # Reset the ID counter.
    cube_vm.CubeVm._CUBE_ID = 0
    # Create a CubeVm object for testing.
    self.__cube = cube_vm.CubeVm()

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
  def test_start(self, mocked_serial, mocked_popen):
    self.__cube.start()

    # Make sure it started the process.
    expected_command = [cube_vm.CubeVm._QEMU_BIN, "-readconfig",
                        cube_vm.CubeVm._QEMU_CONFIG, "-nographic", "-chardev",
                        "socket,path=/tmp/cube0,server,nowait,id=vcube_ser"]
    mocked_popen.assert_called_once_with(expected_command,
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)

    # It should have started the serial interface.
    self.__serial = mocked_serial.assert_called_once_with("/tmp/cube0")

  @mock.patch("subprocess.Popen")
  @mock.patch("simulator.virtual_cube.serial_com.SerialCom")
  def test_stop(self, mocked_serial, mocked_popen):
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


if __name__ == "__main__":
  unittest.main()
