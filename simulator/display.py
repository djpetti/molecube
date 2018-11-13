#import colors
import obj_canvas
from config import *


class Display(obj_canvas.Shape):
  """ Simulates the display hardware for the cube. """
  # TODO (danielp): Re-implement this with a better class hierarchy, possibly
  # with a generic "Container" superclass.

  def __init__(self, canvas, pos, size):
    """
    Args:
      canvas: The canvas to draw on.
      pos: The initial position of the display.
      size: The size of the display. """
    self.__size = size

    # Object representing the display background.
    self.__background = None
    # List of all the canvas objects on-screen.
    self.__display_objs = []

    # Draw on the canvas.
    super(Display, self).__init__(canvas, pos, fill=SCREEN,
                                  outline=SCREEN)

  def _draw_object(self):
    # Draw the background.
    pos = (self._pos_x, self._pos_y)
    self.__background = obj_canvas.Rectangle(self._canvas, pos, self.__size,
                                             fill=self._fill,
                                             outline=self._outline)

    self.__display_objs.append(self.__background)
    # In this case, reference just points to the background.
    self._reference = self.__background._reference

  def get_bbox(self):
    # To implement this, we can just use the bbox of the background.
    return self.__background.get_bbox()

  def move(self, x_shift, y_shift):
    self._pos_x += x_shift
    self._pos_y += y_shift

    for item in self.__display_objs:
      item.move(x_shift, y_shift)

  def delete(self):
    for item in self.__display_objs:
      item.delete()

    self.__display_objs = []
    self._reference = None

  def draw_text(self, text, pos, size):
    """ Draws text on the display.
    Args:
      text: The text to draw.
      pos: The position on the display to draw at.
      size: The size of the text. """
    font = ("Baumans", size)

    # The pos is relative to the screen center.
    screen_x, screen_y = self.__background.get_pos()
    rel_x = screen_x + pos[0]
    rel_y = screen_y + pos[0]
    rel_pos = (rel_x, rel_y)

    item = obj_canvas.Text(self._canvas, rel_pos, text, font)

    # Add to the list of display objects.
    self.__display_objs.append(item)

  def clear(self):
    """ Clears all objects from the display. """
    for item in self.__display_objs:
      if item == self.__background:
        # Don't delete the background.
        continue

      item.delete()

    self.__display_objs = [self.__background]
