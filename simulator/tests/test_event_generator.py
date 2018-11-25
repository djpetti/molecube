import mock
import unittest

from simulator import event, event_generator


class TestGraphicsEventGenerator(unittest.TestCase):
  """ Tests for the GraphicsEventGenerator class. """

  def setUp(self):
    # Create an event generator for testing. We use a mock object to substitute
    # for the canvas.
    self.__mocked_canvas = mock.MagicMock()
    self.__generator = \
        event_generator.GraphicsEventGenerator(self.__mocked_canvas)

  def test_dispatch(self):
    """ Tests that dispatch() works under normal conditions. """
    # Create a fake message to dispatch.
    message = mock.MagicMock()
    graphics_message = message.graphics
    graphics_message.op_type = "TEST_TYPE"
    graphics_message.image.width = 100
    graphics_message.image.height = 200
    graphics_message.image.data = "image_data"

    self.__generator.dispatch(0, message)

    # It should have dispatched the event correctly.
    event_name = event.GraphicsEvent.get_identifier()
    self.__mocked_canvas.generate_event \
        .assert_called_once_with(event_name, cube_id=0,
                                 op_type=graphics_message.op_type,
                                 image_width=graphics_message.image.width,
                                 image_height=graphics_message.image.height,
                                 image_data=graphics_message.image.data)


if __name__ == "__main__":
  unittest.main()
