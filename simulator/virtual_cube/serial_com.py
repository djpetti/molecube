import collections
import logging

import serial

from apps.libmc.sim.protobuf import system_action_pb2

import cows


logger = logging.getLogger(__name__)


class SerialCom(object):
  """ Manages the serial link with the virtual cube. """

  # Baudrate to use for serial connections.
  _BAUDRATE = 115200
  # Separator for serial messages.
  _SEPARATOR = b"\x00\x00"

  def __init__(self, serial_fd):
    """
    Args:
      serial_fd: The serial FD to connect to the cube VM on. """
    logger.info("Connecting to serial device %s." % (serial_fd))
    self.__serial = serial.Serial(serial_fd, self._BAUDRATE)

    # Whether we've found a packet boundary yet.
    self.__packet_synced = False
    # Buffer for incomplete data that we've received.
    self.__data = bytearray()
    # Queue for complete messages that we've received.
    self.__message_queue = collections.deque()

    # Send the initial message separator.
    self.__write_all(self._SEPARATOR)

  def __write_all(self, message):
    """ Blocks until an entire message is written to the serial port.
    Args:
      message: The message to write. """
    remaining = len(message)

    while remaining > 0:
      partial = message[-remaining:]
      remaining -= self.__serial.write(partial)

  def __sync_to_packet(self):
    """ Synchronizes to the packet separator. """
    logger.debug("Synchronizing to packets...")

    # Try to read the packet separator.
    read = self.__serial.read(2)
    while read != self._SEPARATOR:
      read = read[-1]
      read += self.__serial.read(1)

  def __read_until_separator(self):
    """ Reads data until it finds a separator. """
    read_this_round = bytearray()

    while len(self.__message_queue) == 0:
      if len(self.__data):
        # Keep the last byte so we don't miss separators that are split across
        # reads.
        read_this_round = self.__data[-1:]
        self.__data.pop()

      # Start off by reading a single byte.
      read_this_round.extend(self.__serial.read(1))
      # Read anything that is in there with it.
      read_this_round.extend(self.__serial.read(self.__serial.in_waiting))

      while self._SEPARATOR in read_this_round:
        # We found the end.
        current_message, next_message = \
            read_this_round.split(self._SEPARATOR, 1)
        self.__data.extend(current_message)
        # Add the complete message to the queue.
        self.__message_queue.appendleft(self.__data[:])

        read_this_round = next_message
        self.__data = bytearray()

      self.__data.extend(read_this_round)

  def write_message(self, message):
    """ Writes a Protobuf message to the serial port.
    Args:
      message: The message to write. """
    logger.debug("Writing message: %s" % (str(message)))

    # Serialize the message.
    bin_message = message.SerializeToString()
    # Stuff the message.
    cows.cows_stuff(bin_message)
    # Add the separator at the end.
    bin_message += self._SEPARATOR

    self.__write_all(bin_message)

  def read_message(self):
    """ Reads a Protobuf message from the serial port.
    Returns:
      The message that it read. """
    if not len(self.__message_queue):
      # We don't have any buffered messages, so we need to receive more.

      if not self.__packet_synced:
        # Sync to the packet stream.
        self.__sync_to_packet()
        self.__packet_synced = True

      # Read from the serial.
      self.__read_until_separator()

    bin_message = self.__message_queue.pop()

    # Unstuff the message.
    cows.cows_unstuff(bin_message)
    # Deserialize the message.
    message = system_action_pb2.SystemAction()
    message.ParseFromString(bin_message)

    logger.debug("Read message: %s" % (str(message)))

    return message