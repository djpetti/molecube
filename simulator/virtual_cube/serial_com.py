import collections
import logging
import select
import socket

from apps.libmc.sim.protobuf import sim_message_pb2

import cows


logger = logging.getLogger(__name__)


class SerialCom(object):
  """ Manages the serial link with the virtual cube. """

  # Separator for serial messages.
  _SEPARATOR = b"\x00\x00"

  @classmethod
  def select_on(self, coms):
    """ Takes a list of SerialComs, and waits until any of them have data ready
    to be read from the serial.
    Args:
      coms: The list of SerialComs to wait on.
    Returns:
      List of SerialComs that have data ready. """
    # Extract the internal sockets.
    sockets = [com.__socket for com in coms]
    logger.debug("Waiting on %d sockets..." % (len(sockets)))

    # Perform the select call.
    readable, _, _, = select.select(sockets, [], [])
    logger.debug("%d sockets are now readable." % (len(readable)))

    return readable

  def __init__(self, serial_fd):
    """
    Args:
      serial_fd: The serial FD to connect to the cube VM on. """
    logger.info("Connecting to socket device %s." % (serial_fd))
    self.__socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    self.__socket.connect(serial_fd)

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
      remaining -= self.__socket.send(partial)

  def __sync_to_packet(self):
    """ Synchronizes to the packet separator. """
    logger.debug("Synchronizing to packets...")

    # Try to read the packet separator.
    read = self.__socket.recv(2)
    while read != self._SEPARATOR:
      read = read[-1]
      read += self.__socket.recv(1)

  def __read_until_separator(self):
    """ Reads data until it finds a separator. """
    read_this_round = bytearray()

    while len(self.__message_queue) == 0:
      if len(self.__data):
        # Keep the last byte so we don't miss separators that are split across
        # reads.
        read_this_round = self.__data[-1:]
        self.__data.pop()

      # Read anything that is available.
      read_this_round.extend(self.__socket.recv())

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
    # Add the overhead word for COWS.
    bin_message = bytearray(2) + bytearray(bin_message)
    # Stuff the message.
    cows.cows_stuff(bin_message)
    # Add the separator at the end.
    complete_message = bin_message + self._SEPARATOR

    self.__write_all(complete_message)

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
    # Deserialize the message, skipping the overhead word.
    message = sim_message_pb2.SimMessage()
    message.ParseFromString(bin_message[2:])

    logger.debug("Read message: %s" % (str(message)))

    return message
