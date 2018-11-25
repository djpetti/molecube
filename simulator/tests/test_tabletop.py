import mock
import unittest

from simulator import event, tabletop


class TestTabletop(unittest.TestCase):
  """ Tests for the Tabletop class. """

  @mock.patch("simulator.obj_canvas.Canvas")
  @mock.patch("simulator.cube_event_handler.CubeEventHandler")
  def setUp(self, mocked_event_handler, mocked_canvas):
    # Initialize a tabletop for testing.
    self.__tabletop = tabletop.Tabletop()

    # Also save the mocks, since we might want them later.
    self.__mocked_canvas = mocked_canvas
    self.__mocked_event_handler = mocked_event_handler

  def test_init(self):
    """ Tests that initialization works under normal conditions. """
    # It should have initialized the canvas.
    self.__mocked_canvas.assert_called_once_with(background=mock.ANY)
    # It should have bound events.
    canvas_instance = self.__mocked_canvas.return_value
    expected_calls = [mock.call(event.MouseDragEvent, mock.ANY),
                      mock.call(event.MouseReleaseEvent, mock.ANY)]
    canvas_instance.bind_event.assert_has_calls(expected_calls)

    # It should have initialized the cube event handler.
    self.__mocked_event_handler.assert_called_once_with(canvas_instance)

  def test_run(self):
    """ Tests that the event loop for cube events works under normal conditions.
    """
    canvas_instance = self.__mocked_canvas.return_value

    # First, we're going to call run(), which should set up the callback.
    self.__tabletop.run()

    # It should have set up the callback.
    canvas_instance.after.assert_called_once_with(0, mock.ANY)
    # It should have proceeded waiting for events.
    canvas_instance.wait_for_events.assert_called_once_with()


if __name__ == "__main__":
  unittest.main()
