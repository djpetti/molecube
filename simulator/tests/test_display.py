import mock
import unittest

import numpy as np

from simulator import display


class TestDisplay(unittest.TestCase):
  """ Tests for the Display class. """

  @mock.patch("simulator.obj_canvas.Image")
  def setUp(self, mocked_image):
    # Save the image mock so we can access it in later tests.
    self.__mocked_image = mocked_image
    # Set a reasonable reference value.
    self.__mocked_image.return_value._reference = 1

    # Create a display for testing.
    self.__display = display.Display("canvas", (500, 500), (100, 100))

    mocked_image.assert_called_once_with("canvas", (500, 500), mock.ANY)

    # Reset the mock so it's in a defined state for future tests.
    self.__mocked_image.reset_mock()

  def test_create(self):
    """ Tests that __init__ works properly. """
    # It should have set the reference properly.
    self.assertEqual(1, self.__display._reference)
    # It should not have tried to update the image.
    self.__mocked_image.return_value.update.assert_not_called()

  def test_get_bbox(self):
    """ Tests that get_bbox() works under normal conditions. """
    mocked_get_bbox = self.__mocked_image.return_value.get_bbox
    mocked_get_bbox.return_value = (1, 2, 3, 4)

    self.assertSequenceEqual((1, 2, 3, 4), self.__display.get_bbox())

    # It should have delegated to the internal image.
    mocked_get_bbox.assert_called_once_with()

  def test_update(self):
    """ Tests that update() works under normal conditions. """
    mocked_update = self.__mocked_image.return_value.update

    # Create a new image to set.
    image = np.zeros((100, 100, 3), dtype=np.uint8)
    self.__display.update(image)

    # It should have updated the image internally.
    mocked_update.assert_called_once_with(image)
    # The reference should not have changed.
    self.assertEqual(1, self.__display._reference)

  def test_clear(self):
    """ Tests that clear() works under normal conditions. """
    mocked_update = self.__mocked_image.return_value.update

    self.__display.clear()

    # It should have updated the image with the background.
    mocked_update.assert_called_once_with(self.__display._Display__background)
    # The reference should not have changed.
    self.assertEqual(1, self.__display._reference)


if __name__ == "__main__":
  unittest.main()
