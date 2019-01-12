import mock
import unittest

import numpy as np

from simulator import obj_canvas


class TestImage(unittest.TestCase):
  """ Tests for the Image class. """

  @mock.patch("Tkinter.PhotoImage")
  @mock.patch("Tkinter.Tk")
  @mock.patch("Tkinter.Canvas")
  def setUp(self, mocked_canvas, mocked_tk, mocked_photo_image):
    # Create a fake canvas to work with.
    self.__canvas = obj_canvas.Canvas(window_width=1000, window_height=1000)
    # It should have created the canvas.
    mocked_tk.assert_called_once()
    mocked_canvas.assert_called_once()

    # Create an image for testing.
    self.__np_image = np.zeros((100, 100, 3), dtype=np.uint8)
    self.__image = obj_canvas.Image(self.__canvas, (500, 500), self.__np_image)

    # Clear any mocks that will be incorporated into the state of the image
    # instance.
    mocked_canvas.reset_mock()
    mocked_canvas.create_image.reset_mock()
    mocked_tk.reset_mock()
    mocked_photo_image.reset_mock()

  @mock.patch("Tkinter.PhotoImage")
  def test_create(self, mocked_photo_image):
    """ Tests that we can initialize the image properly. """
    mocked_canvas = self.__canvas.get_raw_canvas()
    mocked_create_image = mocked_canvas.create_image
    # Create a reasonable reference value to return.
    mocked_create_image.return_value = 1
    mocked_put = mocked_photo_image.return_value.put

    # Create a new image.
    image = obj_canvas.Image(self.__canvas, (500, 500), self.__np_image)

    # It should have created a new PhotoImage.
    mocked_photo_image.assert_called_once_with(width=100, height=100)
    # It should have set the proper data.
    expected_data = obj_canvas.Image._convert_numpy(self.__np_image)
    mocked_put.assert_called_once_with(expected_data, (0, 0, 99, 99))

    # It should have created the image and set the reference.
    mocked_create_image.assert_called_once_with(500, 500,
        image=mocked_photo_image.return_value)
    self.assertEqual(1, image._reference)

  def test_create_bad_kwargs(self):
    """ Tests that create() handles invalid keyword arguments. """
    self.assertRaises(ValueError, obj_canvas.Image, self.__canvas, (500, 500),
                      self.__np_image, fill="red")
    self.assertRaises(ValueError, obj_canvas.Image, self.__canvas, (500, 500),
                      self.__np_image, outline="red")

  def test_get_bbox(self):
    """ Tests that get_bbox() works under normal conditions. """
    bbox = self.__image.get_bbox()
    self.assertSequenceEqual((450, 450, 550, 550), bbox)

  def test_update(self):
    """ Tests that update() works under normal conditions. """
    mocked_canvas = self.__canvas.get_raw_canvas()
    mocked_put = self.__image._Image__photo_image.put
    mocked_create_image = mocked_canvas.create_image
    mocked_create_image.return_value = 2

    # Create a new image.
    image = np.ones((100, 100, 3), dtype=np.uint8)

    # Set the image.
    self.__image.update(image)

    # It should have set the proper data.
    expected_data = obj_canvas.Image._convert_numpy(image)
    mocked_put.assert_called_once_with(expected_data, (0, 0, 99, 99))

    # It should have created the image and set the reference.
    mocked_create_image.assert_called_once_with(500, 500, image=mock.ANY)
    self.assertEqual(2, self.__image._reference)


if __name__ == "__main__":
  unittest.main()
