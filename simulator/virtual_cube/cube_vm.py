import gzip
import logging
import os
import subprocess


logger = logging.getLogger(__name__)


class CubeVm(object):
  """ This is a wrapper around the QEMU instance. It generally handles stopping,
  starting, and managing the QEMU VM. """

  # TODO (danielp): Put these into a config file eventually.
  # Location of the QEMU binary to use.
  _QEMU_BIN = "/usr/bin/qemu-system-arm"
  # Location of the QEMU configuration file for cubes.
  _QEMU_CONFIG = "simulator/assets/cube_vm.cfg"
  # Location of the image for VMs.
  _DISK_IMAGE = "simulator/assets/cube_os.ext4"

  # Internal counter to use for generating unique cube IDs.
  _CUBE_ID = 0

  def __init__(self):
    # No currently-running process.
    self.__process = None

    # Assign an ID to this cube.
    self.__id = CubeVm._CUBE_ID
    CubeVm._CUBE_ID += 1
    # Assign a name for the serial port.
    self.__serial_name = "cube%d" % (self.__id)

    # Extract the disk image if necessary.
    self.__extract_disk_image()

  def __make_serial_options(self, name):
    """ Creates the QEMU CLI option list for the virtual serial device.
    Args:
      name: Unique name of the serial device. """
    options = ["-chardev",
               "socket,path=/tmp/%s,server,nowait,id=vcube_ser" % (name)]
    return options

  def __extract_disk_image(self):
    """ Extracts the VM disk image if necessary. """
    # The compressed path is just the normal one with a gzip extension.
    compressed_path = CubeVm._DISK_IMAGE + ".gz"

    if not os.path.exists(CubeVm._DISK_IMAGE):
      # We need to extract the compressed version.
      logger.info("Extracting disk image...")

      # Create uncompressed file.
      uncompressed_file = open(CubeVm._DISK_IMAGE, "wb")

      with gzip.open(compressed_path, "rb") as compressed:
        image_content = compressed.read()
        uncompressed_file.write(image_content)
      uncompressed_file.close()

  def start(self):
    """ Starts the cube VM running. """
    command = [self._QEMU_BIN, "-readconfig", self._QEMU_CONFIG, "-nographic"]
    # Add serial options.
    options = self.__make_serial_options(self.__serial_name)
    command.extend(options)

    logger.debug("Running command: %s" % str(command))

    self.__process = subprocess.Popen(command, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE)
    logger.info("Started cube VM %d." % (self.__id))

  def stop(self):
    """ Halts the cube VM. """
    if self.__process is None:
      # Process is not running.
      return

    # Terminate the process.
    logger.info("Terminating cube VM %d." % (self.__id))
    self.__process.communicate("\nhalt\n")

    self.__process = None

  def get_serial(self):
    """ Gets the serial FD for this cube.
    Returns:
      The serial FD for the cube. """
    return "/tmp/%s" % (self.__serial_name)
