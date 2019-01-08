import logging

from config import config
from cube import Cube
from obj_canvas import Line
import cube_event_handler
import display
import event
import obj_canvas


logger = logging.getLogger(__name__)

# Load the simulator config.
sim_config = config.simulator_config()


class Tabletop(object):
  """ Simulates a "tabletop" in which the cubes exist. """

  def __init__(self):
    logger.info("Creating new tabletop")

    # List of cubes.
    grid_width = sim_config.get("appearance", "grid_width")
    grid_height = sim_config.get("appearance", "grid_height")
    self.__cubes = [[None for x in range(grid_width)]
                     for y in range(grid_height)]

    # List of lines making up the grid
    self.__grid = []
    self.__drawngrid = False

    # Canvas on which to draw cubes.
    background_color = sim_config.get("appearance", "colors", "background")
    self.__canvas = obj_canvas.Canvas(background=background_color)
    self.__cube_event_handler = \
        cube_event_handler.CubeEventHandler(self.__canvas)

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

    # Places the cube
    cube_size = sim_config.get("appearance", "cube_size")
    cube_offset = sim_config.get("appearance", "cube_offset")
    selected_cube.snap_to_grid(cube_size,
                               self.__cubes,
                               offset=cube_offset)
    selected_cube.clear_drag()

    # Clear the grid
    self.clear_grid()
    self.__drawngrid = False

  def __mouse_dragged(self, event):
    """ Called when the user drags with the mouse. """
    # Get the currently-selected cube.
    selected_cube = Cube.get_selected()
    if selected_cube is None:
      # No cube is selected. Do nothing.
      return

    # Shows the grid if necessary
    if (self.__drawngrid == False):
      self.__grid = self.draw_grid()
      self.__drawngrid = True

    # Move the cube.
    selected_cube.drag(event)

  def __wait_for_cube_events(self):
    """ Waits for events originating from the cubes, and sent over the serial
    interface. When it receives a message, it dispatches a custom Tkinter event
    which can then be handled in the main thread. """
    while True:
      # Handle all events.
      self.__cube_event_handler.handle_events(self.__cubes)

  def make_cube(self, color=sim_config.get("appearance", "colors", "cube_red")):
    """ Adds a new cube to the canvas.
    Args:
      color: The color of the cube.
    Returns:
      The cube that it made. """
    logger.info("adding a cube to our tabletop")

    cube = Cube(self.__canvas, (0, 0), color)
    x, y = 0, 0
    while self.__cubes[y][x]:
      x += 1
    self.__cubes[y][x] = cube
    cube.set_idx(x, y, self.__cubes)

    return cube

  def get_cubes(self):
    #return the list of cubes we have
    return self.__cubes

  def start_app_on_all(self, app_type):
    """ Starts an application on all the cubes.
    Args:
      app_type: The class of the app to start. """

    message = "starting " + str(app_type) + " on all cubes on our tabletop"
    logger.info(message)

    for cube in self.__cubes:
      # Make a new instance for this cube.
      app = app_type()
      # Run it.
      cube.run_app(app)

  def run(self):
    """ Runs the tabletop simulation indefinitely. """
    # Kick off cube event listener thread.
    self.__canvas.after(0, self.__wait_for_cube_events)

    self.__canvas.wait_for_events()

  def draw_grid(self):
    """ Creates Line objects for grid
    Returns:
      The list of line objects in the grid."""
    grid = []
    window_x, window_y = self.__canvas.get_window_size()
    i = 0

    grid_color = sim_config.get("appearance", "colors", "grid")
    cube_size = sim_config.get("appearance", "cube_size")
    while i < window_x:
      line = Line(self.__canvas, (i, 0), (i, window_y), fill=grid_color)
      grid.append(line)
      i += cube_size
    j = 0
    while j < window_y:
      line = Line(self.__canvas, (0, j), (window_x, j), fill=grid_color)
      grid.append(line)
      j += cube_size
    return grid

  def clear_grid(self):
    """ Removes line ID's from grid, deletes the lines from canvas."""
    for i in range(len(self.__grid)):
      self.__grid[i].delete()
