import numpy as np

from config import config
import obj_canvas


# Load the simulator config.
sim_config = config.simulator_config()


class Display(obj_canvas.Shape):
  """ Simulates the display hardware for the cube. """
  # TODO (danielp): Possibly make some sort of generic Container superclass and
  # inherit from that.

  def __init__(self, canvas, pos, size):
    """
    Args:
      canvas: The canvas to draw on.
      pos: The initial position of the display.
      size: The size of the display. """
    self.__size = size

    # Generate default background.
    self.__background = self.__gen_background()
    # The image currently being displayed.
    self.__np_image = self.__background
    self.__image = None

    # Draw on the canvas.
    super(Display, self).__init__(canvas, pos)

  def __gen_background(self):
    """ Generates the default background image.
    Returns:
      The background image. """
    # The default screen is just a solid color.
    screen_color_hex = sim_config.get("appearance", "colors", "screen")

    # Convert back to RGB.
    screen_color = screen_color_hex.lstrip("#")
    channels = [screen_color[i:i + 2] for i in range(0, len(screen_color), 2)]
    screen_color = [int(channel, 16) for channel in channels]

    # Create solid image.
    width, height = self.__size
    return np.tile(screen_color, (height, width, 1))

  def _draw_object(self):
    # Draw the screen.
    if not self.__image:
      # Create the new image.
      pos = (self._pos_x, self._pos_y)
      self.__image = obj_canvas.Image(self._canvas, pos, self.__np_image)
    else:
      # Update the old one.
      self.__image.update(self.__np_image)

    # Since this entire class essentially wraps an Image object, the reference
    # should just alias to the image reference.
    self._reference = self.__image._reference

  def get_bbox(self):
    # To implement this, we can just use the bbox of the internal image.
    return self.__image.get_bbox()

  def update(self, image):
    """ Updates the displayed image.
    Args:
      image: The new image to use. Should be a Numpy array of shape (w, h, 3).
    """
    self.__np_image = image
    self._draw_object()

  def clear(self):
    """ Resets the display with the default background. """
    self.__np_image = self.__background
    self._draw_object()
