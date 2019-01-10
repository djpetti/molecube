import mock
import unittest

from apps.libmc.sim.protobuf import test_pb2

from simulator.virtual_cube import serial_com


_SEPARATOR = serial_com.SerialCom._SEPARATOR
_MAX_READ = serial_com.SerialCom._MAX_READ


class TestSerialCom(unittest.TestCase):
  """ Tests for the SerialCom class. """

  # Test device to use.
  _TEST_DEVICE = "/dev/serial"

  class FakeCowsUnstuff:
    """ Fake implementation of cows_unstuff that swaps out the passed array for
    a valid message. """

    def __init__(self, message):
      """
      Args:
        message: The valid message that we want to substitute. If message is a
                 list, it will substitute the next one on each call. """
      self.__message = message
      self.__call_count = 0

    def __call__(self, array):
      # Get the correct one if we have a sequence.
      message = self.__message
      if type(message) is list:
        message = message[self.__call_count]
      self.__call_count += 1

      # The array should be one word longer than the message due to the
      # overhead.
      assert len(array) == len(message) + 2

      # Copy the legitimate message.
      for i in range(0, len(message)):
        array[i + 2] = message[i]

  def __fake_message(self, length):
    """ Creates a fake message of a given length, including the packet
    separator and overhead word. (Not included in the length.)
    Args:
      length: The length of the message.
    Returns:
      The fake message. """
    return b'1' * (length + 2) + _SEPARATOR

  # Patch the serial call so it doesn't try to open an actual device.
  @mock.patch("socket.socket")
  def __make_serial(self, mocked_socket):
    """ Create a new SerialCom object for testing.
    Args:
      mocked_socket: The mocked socket instance.
    Returns:
      The created SerialCom object, and the mocked socket instance. """
    # Make sure we can write to the socket.
    mocked_inst = mocked_socket.return_value
    mocked_inst.send.return_value = 2

    # Create a SerialCom object for testing.
    serial = serial_com.SerialCom(self._TEST_DEVICE)

    # Check that the socket was created and connected.
    mocked_socket.assert_called_once()
    mocked_inst.connect.assert_called_once_with(self._TEST_DEVICE)
    # Check that it wrote the intial packet separator.
    mocked_inst.send.assert_called_once_with(_SEPARATOR)
    # Reset the mock so it behaves the way we expect in future tests.
    mocked_inst.reset_mock()

    return serial, mocked_inst

  def test_open(self):
    """ Tests that we can open the serial link under normal conditions. """
    self.__make_serial()

  @mock.patch("simulator.virtual_cube.cows.cows_stuff")
  def test_write(self, mocked_cows):
    """ Tests that we can write a message under normal conditions. """
    # Create a message to write.
    message = test_pb2.TestMessage()
    message.field1 = 42
    message.field2 = True

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like the write succeeded in one go.
    mocked_socket.send.return_value = message.ByteSize() + 2 + len(_SEPARATOR)
    # For the cows stuffing, we're just not going to do anything.

    com.write_message(message)

    # It should have written the message, plus an overhead word.
    bin_message = message.SerializeToString()
    mocked_cows.assert_called_once_with(bytearray(2) + bin_message)
    mocked_socket.send.assert_called_once_with(bytearray(2) + bin_message + _SEPARATOR)

  @mock.patch("simulator.virtual_cube.cows.cows_stuff")
  def test_write_partial_message(self, mocked_cows):
    """ Tests that we can write a message when the message gets broken up across
    multiple writes. """
    # Create a message to write.
    message = test_pb2.TestMessage()
    message.field1 = 42
    message.field2 = True

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like the write succeeded in two tries. (Add two bytes for the
    # overhead.)
    mocked_socket.send.side_effect = [message.ByteSize() + 2, len(_SEPARATOR)]
    # For the cows stuffing, we're just not going to do anything.

    com.write_message(message)

    # It should have written the message.
    bin_message = message.SerializeToString()
    mocked_cows.assert_called_once_with(bytearray(2) + bin_message)
    # It should have made two calls to write.
    expected_calls = [mock.call.send(bytearray(2) + bin_message + _SEPARATOR),
                      mock.call.send(_SEPARATOR)]
    mocked_socket.assert_has_calls(expected_calls)

  @mock.patch("simulator.virtual_cube.cows.cows_unstuff")
  @mock.patch("apps.libmc.sim.protobuf.sim_message_pb2.SimMessage")
  def test_read_message(self, mocked_sys_action, mocked_cows):
    """ Tests that we can read a message under normal conditions. """
    # Create the message to read.
    message = test_pb2.TestMessage()
    message.field1 = 42
    message.field2 = True
    mocked_sys_action.return_value = message
    bin_message = message.SerializeToString()

    # Configure the fake cows implementation.
    fake_cows = self.FakeCowsUnstuff(bin_message)
    mocked_cows.side_effect = fake_cows

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like synchronizing to the packet boundary worked. It will
    # then attempt to read the entire message.
    fake_message = self.__fake_message(len(bin_message))
    mocked_socket.recv.side_effect = [_SEPARATOR, fake_message]

    # Try to read the message.
    got_message = com.read_message()

    # The first call to read() should have been for the initial separator. The
    # second call should have been for the message and ending separator.
    expected_calls = [mock.call.recv(len(_SEPARATOR)),
                      mock.call.recv(_MAX_READ)]
    mocked_socket.assert_has_calls(expected_calls)
    # It should have tried to unstuff it.
    mocked_cows.assert_called_once()

    # Make sure the message matches.
    self.assertEqual(message, got_message)

  @mock.patch("simulator.virtual_cube.cows.cows_unstuff")
  @mock.patch("apps.libmc.sim.protobuf.sim_message_pb2.SimMessage")
  def test_read_message_split(self, mocked_sys_action, mocked_cows):
    """ Tests that we can read a message when it is split across multiple read
    calls. """
    # Create the message to read.
    message = test_pb2.TestMessage()
    message.field1 = 42
    message.field2 = True
    mocked_sys_action.return_value = message
    bin_message = message.SerializeToString()

    # Configure the fake cows implementation.
    fake_cows = self.FakeCowsUnstuff(bin_message)
    mocked_cows.side_effect = fake_cows

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like synchronizing to the packet boundary worked. It will
    # attempt to read the entire message afterwards, but initially, it will only
    # get the first byte. It will then have to make another call to get the
    # rest.
    fake_message = self.__fake_message(len(bin_message))
    mocked_socket.recv.side_effect = [_SEPARATOR, fake_message[0],
                                      fake_message[1], fake_message[2:]]

    # Try to read the message.
    got_message = com.read_message()

    # The first call to recv() should have been for the initial separator. The
    # second call should have been for the first byte of the message, the
    # third call should have been for reading the second byte of the message,
    # and the fourth call should have been for reading the rest of the message
    # and the next separator.
    expected_calls = [mock.call.recv(len(_SEPARATOR)),
                      mock.call.recv(_MAX_READ),
                      mock.call.recv(_MAX_READ),
                      mock.call.recv(_MAX_READ)]
    mocked_socket.assert_has_calls(expected_calls)
    # It should have tried to unstuff it.
    mocked_cows.assert_called_once()

    # Make sure the message matches.
    self.assertEqual(message, got_message)

  @mock.patch("simulator.virtual_cube.cows.cows_unstuff")
  @mock.patch("apps.libmc.sim.protobuf.sim_message_pb2.SimMessage")
  def test_read_message_buffered(self, mocked_sys_action, mocked_cows):
    """ Tests that we can read a message when we get an extra buffered one. """
    # Create two messages to read.
    message1 = test_pb2.TestMessage()
    message1.field1 = 42
    message1.field2 = True
    bin_message1 = message1.SerializeToString()

    message2 = test_pb2.TestMessage()
    message2.field1 = 1337
    message2.field2 = True
    bin_message2 = message2.SerializeToString()

    # Configure the fake cows implementation.
    fake_cows = self.FakeCowsUnstuff([bin_message1, bin_message2])
    mocked_cows.side_effect = fake_cows

    mocked_sys_action.return_value = message1

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like synchronizing to the packet boundary worked. It will
    # attempt to read the first message first. In this case, it will get the
    # second message as well.
    fake_message1 = self.__fake_message(len(bin_message1))
    fake_message2 = self.__fake_message(len(bin_message2))
    mocked_socket.recv.side_effect = [_SEPARATOR,
                                      fake_message1 + fake_message2]


    # Try to read the first message.
    got_message1 = com.read_message()

    # The first call to recv() should have been for the initial separator. The
    # second call should have been for the first message, ending separator,
    # and second message and its ending separator.
    expected_calls = [mock.call.recv(len(_SEPARATOR)),
                      mock.call.recv(_MAX_READ)]
    mocked_socket.assert_has_calls(expected_calls)
    mocked_socket.recv.reset_mock()
    # It should have tried to unstuff it.
    mocked_cows.assert_called_once()
    mocked_cows.reset_mock()

    # Make sure the message matches.
    self.assertEqual(message1, got_message1)

    # We should be able to call read_message again and essentially get the next
    # message for free.
    got_message2 = com.read_message()

    # There should be no calls to the recv method.
    mocked_socket.recv.assert_not_called()
    # It should have tried to unstuff the message.
    mocked_cows.assert_called_once()

    # Make sure the message matches.
    self.assertEqual(message2, got_message2)

  @mock.patch("simulator.virtual_cube.cows.cows_unstuff")
  @mock.patch("apps.libmc.sim.protobuf.sim_message_pb2.SimMessage")
  def test_read_message_continued(self, mocked_sys_action, mocked_cows):
    """ Tests that we can read a message when we read part of the next one. """
    # Create two messages to read.
    message1 = test_pb2.TestMessage()
    message1.field1 = 42
    message1.field2 = True
    bin_message1 = message1.SerializeToString()

    message2 = test_pb2.TestMessage()
    message2.field1 = 1337
    message2.field2 = True
    bin_message2 = message2.SerializeToString()

    # Configure the fake cows implementation.
    fake_cows = self.FakeCowsUnstuff([bin_message1, bin_message2])
    mocked_cows.side_effect = fake_cows

    mocked_sys_action.return_value = message1

    # Create the SerialCom object.
    com, mocked_socket = self.__make_serial()

    # Make it look like synchronizing to the packet boundary worked. It will
    # attempt to read the first byte of the message first, and then it will read
    # the rest. In this case, it will get part of the second message as well.
    fake_message1 = self.__fake_message(len(bin_message1))
    fake_message2 = self.__fake_message(len(bin_message2))
    mocked_socket.recv.side_effect = [_SEPARATOR,
                                      fake_message1 + fake_message2[0]]

    # Try to read the first message.
    got_message1 = com.read_message()

    # The first call to read() should have been for the initial separator. The
    # second call should have been for the first message, ending separator, and
    # first byte of the second message.
    expected_calls = [mock.call.recv(len(_SEPARATOR)),
                      mock.call.recv(_MAX_READ)]
    mocked_socket.assert_has_calls(expected_calls)
    mocked_socket.recv.reset_mock()
    # It should have tried to unstuff it.
    mocked_cows.assert_called_once()
    mocked_cows.reset_mock()

    # Make sure the message matches.
    self.assertEqual(message1, got_message1)

    # When we read the next message, it will read the entire reset of the
    # message including the ending separator.
    mocked_socket.recv.side_effect = [fake_message2[1:]]

    # Try to read the second message.
    got_message2 = com.read_message()

    # It should have made one call to recv() which should have gotten the end of
    # the second message plus the ending separator.
    mocked_socket.recv.assert_called_once_with(_MAX_READ)
    # It should have tried to unstuff it.
    mocked_cows.assert_called_once()

    # Make sure the message matches.
    self.assertEqual(message2, got_message2)

  @mock.patch("simulator.virtual_cube.cows.cows_unstuff")
  @mock.patch("apps.libmc.sim.protobuf.sim_message_pb2.SimMessage")
  def test_sync_to_packet(self, mocked_sys_action, mocked_cows):
    """ Tests that we can successfully synchronize to the packet boundary when
    it requires multiple tries. """
    com, mocked_socket = self.__make_serial()

    # Fake the cows unstuffing so we get a valid message.
    message = test_pb2.TestMessage()
    bin_message = message.SerializeToString()
    mocked_sys_action.return_value = message
    fake_cows = self.FakeCowsUnstuff(bin_message)
    mocked_cows.side_effect = fake_cows

    fake_message = self.__fake_message(len(bin_message))

    # Mock the read function so it looks like finding the separator failed and
    # then succeeded. The final 2 calls are just dummy separators so that the
    # read terminates.
    mocked_socket.recv.side_effect = [b'10', b'1', _SEPARATOR[0], _SEPARATOR[1],
                                      fake_message + _SEPARATOR]

    # If we try to read now, it should first synchronize to the packet.
    got_message = com.read_message()

    # Make sure it tried to call read the proper number of times.
    expected_calls = [mock.call.recv(len(_SEPARATOR)),
                      mock.call.recv(1), mock.call.recv(1),
                      mock.call.recv(1), mock.call.recv(_MAX_READ)]
    mocked_socket.assert_has_calls(expected_calls)
    # It should have tried to unstuff the message.
    mocked_cows.assert_called_once()

    # Make sure the messages match.
    self.assertEqual(message, got_message)

  @mock.patch("select.select")
  def test_select_on(self, mocked_select):
    """ Tests that select_on() works under normal conditions. """
    # Make some instances to try with.
    coms = []
    for i in range(0, 3):
      com, _ = self.__make_serial()
      coms.append(com)

    # Make sure the select call indicates that something is readable.
    mocked_select.return_value = ([coms[0]._SerialCom__socket], [], [])

    readable = serial_com.SerialCom.select_on(coms)
    # It should have given us the correct one.
    self.assertListEqual([coms[0]], readable)

    # It should have called select().
    mocked_select.assert_called_once()


if __name__ == "__main__":
  unittest.main()
