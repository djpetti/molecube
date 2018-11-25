import gzip
import logging
import os
import subprocess
import time

from apps.libmc.sim.protobuf import sim_message_pb2

import serial_com


logger = logging.getLogger(__name__)


def _has_child_process(func):
    """ Decorator that first checks to make sure that we have a child process
    before running func.
    Args:
      func: The function to decorate. """
    def wrapper(*args):
      # Expect the first argument to be the instance.
      self = args[0]
      if not self._child_process:
        raise RuntimeError("Cannot call this with attached process.")

      func(*args)

    return wrapper


class CubeVm(object):
  """ This is a wrapper around the QEMU instance. It generally handles stopping,
  starting, and managing the QEMU VM. """

  # TODO (danielp): Put these into a config file eventually.
  # Location of the QEMU binary to use.
  _QEMU_BIN = "/usr/bin/qemu-system-arm"
  # Location of the QEMU configuration file for cubes.
  _QEMU_CONFIG = "simulator/virtual_cube/assets/cube_vm.cfg"
  # Location of the image for VMs.
  _DISK_IMAGE = "simulator/virtual_cube/assets/cube_os.ext4"

  # Internal counter to use for generating unique cube IDs.
  _CUBE_ID = 0

  @classmethod
  def select_on(self, cubes):
    """ Takes a list of VMs, and waits until any of them have data ready to be
    read from the serial.
    Args:
      cubes: The list of VMs to wait on.
    Returns:
      List of cubes that have data ready. """
    # Extract the serial interfaces from the cubes.
    serials = {}
    for cube in cubes:
      if cube.__serial is None:
        raise AttributeError( \
            "Cannot run select_on() with VMs that are stopped.")
      serials[cube.__serial] = cube

    # Delegate to the serial manager.
    readable = serial_com.SerialCom.select_on(serials.keys())

    # Extract the VMs.
    return [serials[serial] for serial in readable]

  def __init__(self, attach_to=None):
    """
    Args:
      attach_to: When set to a serial handle, it will attach to that
                 currently-running instance instead of creating a new one. """
    # No currently-running process.
    self.__process = None

    # Assign an ID to this cube.
    self.__id = CubeVm._CUBE_ID
    CubeVm._CUBE_ID += 1
    # Assign a name for the serial port.
    self.__serial_name = "cube%d" % (self.__id)

    # Manages communication over the serial link. We initialize this upon
    # creation of the VM.
    self.__serial = None

    # Extract the disk image if necessary.
    self.__extract_disk_image()

    self._child_process = True
    if attach_to:
      # Attach to a running instance.
      logger.info("Attaching to instance '%s'." % (attach_to))
      self.__serial_name = attach_to
      self._child_process = False

      # Create the serial link manager.
      self.__serial = serial_com.SerialCom(self.get_serial())

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

  @_has_child_process
  def start(self):
    """ Starts the cube VM running. """
    if self.__process is not None:
      raise RuntimeError("Process is already started.")

    command = [self._QEMU_BIN, "-readconfig", self._QEMU_CONFIG, "-nographic"]
    # Add serial options.
    options = self.__make_serial_options(self.__serial_name)
    command.extend(options)

    logger.debug("Running command: %s" % str(command))

    self.__process = subprocess.Popen(command, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE)
    logger.info("Started cube VM %d." % (self.__id))

    # Wait for the serial interface to exist.
    logger.debug("Waiting for serial...")
    while not os.path.exists(self.get_serial()):
      time.sleep(1)

    # Create the serial link manager.
    self.__serial = serial_com.SerialCom(self.get_serial())

  def stop(self):
    """ Halts the cube VM. """
    if (self._child_process and self.__process is None):
      # Process is not running.
      return

    # Terminate the process. We do this by sending a special shutdown message.
    logger.info("Terminating cube VM %d." % (self.__id))
    sim_message = sim_message_pb2.SimMessage()
    sim_message.system.shutdown = True

    self.send_message(sim_message)

    if self._child_process:
      # Wait for the child to terminate.
      logger.info("Waiting for VM to exit...")
      self.__process.wait()
      self.__process = None

  def get_serial(self):
    """ Gets the path to the serial device for this cube.
    Returns:
      The serial FD for the cube. """
    return "/tmp/%s" % (self.__serial_name)

  def send_message(self, message):
    """ Sends a message to this cube.
    Args:
      message: The message to send. This must be a protobuf SimMessage. """
    self.__serial.write_message(message)

  def receive_message(self):
    """ Receives a message from this cube. Will block until it receives
    something.
    Returns:
      The message that it received. """
    return self.__serial.read_message()
