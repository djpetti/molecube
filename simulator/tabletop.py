import sys

import colors
import display
import event
import obj_canvas
from cube import Cube

class Tabletop(object):
  """ Simulates a "tabletop" in which the cubes exist. """

  GRID_SIZE = Cube.CUBE_SIZE
  GRID_WIDTH, GRID_HEIGHT = 8, 4
  GRID_OFFSET = GRID_SIZE / 2

  def __init__(self):
    # List of cubes.
    self.__cubes = [[None for x in range(Tabletop.GRID_WIDTH)]
                     for y in range(Tabletop.GRID_HEIGHT)]

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
    self.clear_grid()
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
      self.__grid = self.draw_grid()
      self.__drawngrid = True
    selected_cube.drag(event)

  def make_cube(self, color=Cube.Colors.RED):
    """ Adds a new cube to the canvas.
    Args:
      color: The color of the cube.
    Returns:
      The cube that it made. """
    cube = Cube(self.__canvas, (0, 0), color)
    x, y = 0, 0
    while self.__cubes[y][x]:
        x += 1
        cube.set_idx(x, y, self.__cubes)
    self.__cubes[y][x] = cube

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
  
  def draw_grid(self):
    grid = []
    window_x, window_y = self.__canvas.get_window_size()
    i = 0 
    while i < window_x:
      id = self.__canvas.add_line( i, 0, i, window_y)
      grid.append(id)
      i+=200
    j = 0
    while j < window_y:
      id = self.__canvas.add_line(0, j, window_x, j)
      grid.append(id)
      j+=200
    return grid
    
  def clear_grid(self):
    for i in range(len(self.__grid)):
      id = self.__grid.pop()
      self.__canvas.delete_object(id)
   
