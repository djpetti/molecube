import mock
import unittest

from simulator import cube_event_handler


class TestCubeEventHandler(unittest.TestCase):
  """ Tests for the CubeEventHandler class. """

  @mock.patch("simulator.event_generator.GraphicsEventGenerator")
  def setUp(self, mocked_graphics_generator):
    # Create an instance to test with, using a fake canvas.
    self.__fake_canvas = mock.Mock()
    self.__handler = cube_event_handler.CubeEventHandler(self.__fake_canvas)

    self.__mocked_graphics_generator = mocked_graphics_generator

  def test_init(self):
    """ Tests that we can initialize the class under normal conditions. """
    # It should have initialized the graphics generators.
    self.__mocked_graphics_generator.assert_called_once_with(self.__fake_canvas)

  @mock.patch("simulator.cube.Cube.select_on")
  def test_handle_events(self, mocked_select):
    """ Tests that handle_events works under normal conditions. """
    # Create some fake cubes.
    cubes = []
    for i in range(0, 3):
      cubes.append(mock.Mock())

    # Make it look like one of them has a message ready.
    mocked_select.return_value = [cubes[0]]

    message = mock.Mock()
    message.HasMessage.return_value = True
    cubes[0].receive_message.return_value = message
    cubes[0].get_id.return_value = 0

    self.__handler.handle_events(cubes)

    # It should have read the message correctly.
    mocked_select.assert_called_once_with(cubes)
    cubes[0].receive_message.assert_called_once_with()
    cubes[0].get_id.assert_called_once_with()

    # It should have dispatched the event.
    message.HasMessage.assert_called_once_with("graphics")
    generator_instance = self.__mocked_graphics_generator.return_value
    generator_instance.dispatch.assert_called_once_with(0, message)


if __name__ == "__main__":
  unittest.main()
