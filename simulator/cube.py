import logging

from virtual_cube import cube_vm

from config import config
import display
import event
import logging
import obj_canvas


logger = logging.getLogger(__name__)

# Get the configuration for the simulator.
sim_config = config.simulator_config()


class Cube(object):
  """ Represents a single cube. """

  class Sides(object):
    """ Represents cube sides. """

    LEFT = "left"
    RIGHT = "right"
    TOP = "top"
    BOTTOM = "bottom"

    # List of each side
    _ALL = {LEFT, RIGHT, BOTTOM, TOP}

    # Associates each side with its opposite.
    _OPPOSITES = {LEFT: RIGHT,
                  RIGHT: LEFT,
                  TOP: BOTTOM,
                  BOTTOM: TOP}

    # Unit vector of coordinate shift needed to find cube at given side
    _COORDINATES = {LEFT: (-1, 0),
                    RIGHT: (1, 0),
                    TOP: (0, -1),
                    BOTTOM: (0, 1)}

    @classmethod
    def opposite(cls, side):
      """ Gets the opposite side to a specified one.
      Args:
        side: The side to get the opposite of.
      Returns:
        The opposite side. """
      return cls._OPPOSITES[side]

    @classmethod
    def coordinates(cls, side):
      """ Gets the unit coordinate shift necessary to reach a cube on the given side
      Args:
        side: The side to get the coordinate shift for
      Returns:
        Unit vector of the coordinate shift as tuple. """
      return cls._COORDINATES[side]

    @classmethod
    def all(cls):
      """ Gets a list of all sides
      Returns:
        A list of all sides """
      return cls._ALL

  # Currently selected cube. There can be only one.
  _selected = None
  # Unique ID for the cube.
  _current_id = 0

  @classmethod
  def select_on(self, cubes):
    """ Takes a list of Cubes, and waits until any of them have data ready to be
    read from the serial.
    Args:
      cubes: The list of Cubes to wait on.
    Returns:
      List of cubes that have data ready. """
    # Extract the VMs from the cubes.
    vms = {}
    for cube in cubes:
      vms[cube.__vm] = cube

    # Delegate to the cube VM.
    readable = cube_vm.CubeVm.select_on(vms.keys())

    # Extract the cubes.
    return [vms[vm] for vm in readable]

  @classmethod
  def get_selected(cls):
    """
    Returns:
      The currently selected cube. """
    return cls._selected

  def __init__(self, canvas, idx, color):
    """
    Args:
      canvas: The canvas to draw the cube on.
      idx: The grid indices where the new cube is located.
      color: The color of the cube. """
    self.__canvas = canvas
    self.__idx = idx

    # Determine position from index
    (x, y) = self.__idx
    cube_size = sim_config.get("appearance", "cube_size")
    cube_offset = sim_config.get("appearance", "cube_offset")
    x *= cube_size
    y *= cube_size
    x += cube_offset
    y += cube_offset
    self.__pos = (x, y)

    self.__color = color

    # Assign a unique ID.
    self.__id = Cube._current_id
    Cube._current_id += 1
    logger.info("Creating new cube with ID %d." % (self.__id))

    # Whether the cube is currently being dragged.
    self.__dragging = False
    # List of shapes in the cube.
    self.__cube_shapes = []
    # List of other cubes that are currently connected to this one. A None
    # in a position indicates that no cube is connected there.
    self.__connected = {Cube.Sides.LEFT: None,
                        Cube.Sides.RIGHT: None,
                        Cube.Sides.TOP: None,
                        Cube.Sides.BOTTOM: None}
    # The current application running on the cube. If None, then no application
    # is running.
    self.__application = None

    # Initialize the VM for the cube.
    self.__vm = cube_vm.CubeVm()
    self.__vm.start()

    self.__draw_cube()

  def __hash__(self):
    # Use the ID to generate a hash.
    return hash(self.get_id())

  def __eq__(self, other):
    if not isinstance(other, Cube):
      # It's valid to compare it to something that's not a cube, in which case
      # they're automatically different.
      return False

    # Two instances are the same if they have the same ID.
    return self.get_id() == other.get_id()

  def __draw_cube(self):
    """ Draws the cube on the canvas. """
    x, y = self.__pos

    # Draw the actual cube shapes.
    base_size = sim_config.get("appearance", "cube_size")
    button_color = sim_config.get("appearance", "colors", "buttons")
    case = obj_canvas.Rectangle(self.__canvas, self.__pos,
                                (base_size, base_size),
                                fill=self.__color, outline=self.__color)
    self.__screen = display.Display(self.__canvas, (x, y - 20), (180, 140))
    button_l = obj_canvas.Rectangle(self.__canvas, (x - 65, y + 75), (50, 30),
                                    fill=button_color,
                                    outline=button_color)
    button_c = obj_canvas.Rectangle(self.__canvas, (x, y + 75), (50, 30),
                                    fill=button_color,
                                    outline=button_color)
    button_r = obj_canvas.Rectangle(self.__canvas, (x + 65, y + 75), (50, 30),
                                    fill=button_color,
                                    outline=button_color)

    self.__cube_shapes.extend([case, self.__screen, button_l, button_c, button_r])

    # Bind mouse events for the cube.
    case.bind_event(event.MousePressEvent, self.__cube_clicked)

  def __cube_clicked(self, event):
    """ Called when the user presses the mouse button over the cube. """
    # We are now dragging this cube.
    self.__dragging = True
    # The cube is now selected.
    Cube._selected = self
    # As soon as the cube is selected, all connections are broken.
    self.__clear_connections()
    # When moving, keeps track of the previous mouse position.
    self.__prev_mouse_x, self.__prev_mouse_y = event.get_pos()

  def __config_changed_hook(self):
    """ Run when the connection configuration changes. It takes care of
    notifying the running application. """
    if self.__application is None:
      # No application.
      return

    self.__application.on_reconfiguration(self.get_connections())

  def __add_connection(self, other, side):
    """ Adds a connection from this cube to another one.
    Args:
      other: The cube to connect to.
      side: The side at which this cube is connected.
    Returns:
      True if the configuration changed, false otherwise. """
    if self.__connected[side] == other:
      return False

    self.__connected[side] = other
    return True

  def __clear_connections(self):
    """ Clears all connections to this cube. """
    changed = False

    for side in self.__connected.iterkeys():
      if self.__connected[side] is not None:
        # Clear the connection on the other side.
        opposite = Cube.Sides.opposite(side)
        self.__connected[side].disconnect(opposite)
        changed = True

      self.__connected[side] = None

    if changed:
      # Report a state change.
      self.__config_changed_hook()

  def __set_pos(self, x, y):
    """ Sets the pixel position of the cube.
    Args:
      x: The new x position.
      y: The new y position. """
    # Figure out the offset from the old position.
    old_x, old_y = self.get_pos()
    move_x = x - old_x
    move_y = y - old_y
    self.__pos = (x, y)

    for shape in self.__cube_shapes:
      shape.move(move_x, move_y)

    # Update the canvas.
    self.__canvas.update()

    message = "cube changed position from " + str((old_x, old_y)) + " to " + str((x, y))
    logger.info(message)

  def get_connections(self):
    """
    Returns:
      A dictionary of all cubes that are currently connected to this one.
    """
    return self.__connected.copy()

  def is_side_clear(self, side):
    """ Checks if a certain side is unconnected on this cube.
    Args:
      side: The side to check. """
    return self.__connected[side] is None

  def disconnect(self, side):
    """ Removes a connection from the cube.
    Args:
      side: The side to disconnect. """
    if self.__connected[side] is None:
      return

    self.__connected[side] = None
    # Report a state change.
    self.__config_changed_hook()

  def get_pos(self):
    """
    Returns:
      The current position of the cube as (x, y). """
    case = self.__cube_shapes[0]
    return case.get_pos()

  def set_idx(self, x, y, others):
    """ Sets the index position of the cube.
    Args:
      x: The new x position.
      y: The new y position. """
    self.__idx = (x, y)
    cube_size = sim_config.get("appearance", "cube_size")
    cube_offset = sim_config.get("appearance", "cube_offset")
    x *= cube_size
    y *= cube_size
    x += cube_offset
    y += cube_offset

    self.__set_pos(x, y)
    self.update_connections(others)

  def get_color(self):
    return self.__color

  def get_display(self):
    """
    Returns:
      The display object for this cube. """
    return self.__screen

  def run_app(self, app):
    """ Run a new application on the cube.
    Args:
      app: The class of the app to run. """
    self.__application = app
    self.__application.run(self)

  def get_app(self):
    """ return the app we are running"""
    return self.__application

  def drag(self, event):
    """ Respond to a mouse drag while the cube is selected.
    Args:
      event: The drag event to respond to. """
    if not self.__dragging:
      # We're not actively dragging this cube. Don't move it.
      return

    # Move the entire cube.
    new_x, new_y = event.get_pos()
    # Figure out how much to move it.
    move_x = new_x - self.__prev_mouse_x
    move_y = new_y - self.__prev_mouse_y
    for shape in self.__cube_shapes:
      shape.move(move_x, move_y)

    self.__prev_mouse_x = new_x
    self.__prev_mouse_y = new_y

  def clear_drag(self):
    """ Clears the current dragging state. """
    if not self.__dragging:
      # Not dragging this cube.
      return

    self.__dragging = False
    assert Cube._selected == self
    Cube._selected = None

  def send_message(self, message):
    """ Sends a message to the cube.
    Args:
      message: The SimMessage to send. """
    self.__vm.send_message(message)

  def receive_message(self):
    """ Receives a message from the cube.
    Returns:
      The message that it received. """
    return self.__vm.receive_message()

  def snap_to_grid(self, grid_size, others, offset = 0):
    """ Snap this cube to grid.
      Args:
        grid_size: pixel size of grid as (w, h)
        others: 2D Array representing all cubes in their locations
        offsest: offset of the snap grid in pixels (x, y) """

    # Snap to proper position
    old_pos = self.get_pos()
    old_idx = self.__idx
    new_x = old_pos[0] - old_pos[0] % grid_size + offset
    new_y = old_pos[1] - old_pos[1] % grid_size + offset

    # makes sure no cube is in the way, moves the cube if so
    (x1, y1) = self.__idx

    size = sim_config.get("appearance", "cube_size")
    offset = sim_config.get("appearance", "cube_offset")

    x2 = (new_x - offset) // size
    y2 = (new_y - offset) // size
    swap_cube = others[y2][x2]
    others[y2][x2] = self
    others[y1][x1] = swap_cube

    if swap_cube:
        swap_cube.__clear_connections()
        swap_cube.set_idx(x1, y1, others)

    self.set_idx(x2, y2, others)

  def update_connections(self, others):
    # Maximum size of the grid.
    max_height = len(others)
    max_width = len(others[0])

    # Check for cubes at each side
    for side in Cube.Sides.all():
      shift = Cube.Sides.coordinates(side)
      other_x, other_y = shift[0] + self.__idx[0], shift[1] + self.__idx[1]
      if (other_x >= max_width or other_x < 0):
        # X dimension is out of range.
        continue
      if (other_y >= max_height or other_y < 0):
        # Y dimension is out of range.
        continue

      other = others[other_y][other_x]

      # If a cube exists on this side, add connections
      if other:
        self_changed = self.__add_connection(other, side)
        other_changed = other.__add_connection(self, Cube.Sides.opposite(side))
        if self_changed:
          self.__config_changed_hook()
        if other_changed:
          other.__config_changed_hook()

  def get_id(self):
    """
    Returns:
      The unique ID for this cube. """
    return self.__id
