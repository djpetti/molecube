import sys

import colors
import display
import event
import obj_canvas
from cube import Cube

class Tabletop(object):
  """ Simulates a "tabletop" in which the cubes exist. """

  GRID_SIZE = Cube.CUBE_SIZE
  GRID_OFFSET = GRID_SIZE/2

  def __init__(self):
    # List of cubes.
    self.__cubes = []
    self.__grid = []
    self.__drawngrid = False

    # Canvas on which to draw cubes.
    self.__canvas = obj_canvas.Canvas()

    # When we drag the mouse, we want to move the currently-selected cube.
    self.__canvas.bind_event(event.MouseDragEvent, self.__mouse_dragged)
    # When we release the mouse button, we want to clear the dragging state for
    # all the cubes.
    self.__canvas.bind_event(event.MouseReleaseEvent, self.__mouse_released)

  def __mouse_released(self, event):
    """ Called when the user releases the mouse button. """
    # Clear the dragging state of the selected cube.
    selected_cube = Cube.get_selected()
    if selected_cube is None:
      # No cube is selected. Do nothing.
      return
    selected_cube.snap_to_grid(Tabletop.GRID_SIZE,
                               self.__cubes,
                               offset = Tabletop.GRID_OFFSET)
    selected_cube.clear_drag()
    self.__canvas.clear_grid(self.__grid)
    self.__drawngrid = False

  def __mouse_dragged(self, event):
    """ Called when the user drags with the mouse. """
    # Get the currently-selected cube.
    selected_cube = Cube.get_selected()
    if selected_cube is None:
      # No cube is selected. Do nothing.
      return

    # Move the cube.
    if (self.__drawngrid == False):
      self.__grid = self.__canvas.draw_grid()
      self.__drawngrid = True
    selected_cube.drag(event)

  def make_cube(self, color=Cube.Colors.RED):
    """ Adds a new cube to the canvas.
    Args:
      color: The color of the cube.
    Returns:
      The cube that it made. """
    cube = Cube(self.__canvas, (100, 100), color)
    self.__cubes.append(cube)

    return cube

  def start_app_on_all(self, app_type):
    """ Starts an application on all the cubes.
    Args:
      app_type: The class of the app to start. """
    for cube in self.__cubes:
      # Make a new instance for this cube.
      app = app_type()
      # Run it.
      cube.run_app(app)

  def run(self):
    """ Runs the tabletop simulation indefinitely. """
    self.__canvas.wait_for_events()
