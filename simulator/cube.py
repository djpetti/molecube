import colors
import display
import event
import obj_canvas

class Cube(object):
  """ Represents a single cube. """

  class Colors(object):
    """ Represents cube colors. """

    RED = colors.CUBE_RED
    BLUE = colors.CUBE_BLUE
    GOLD = colors.CUBE_GOLD

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

  # Base cube size, in px.
  CUBE_SIZE = 200
  CUBE_OFFSET = CUBE_SIZE//2

  # Currently selected cube. There can be only one.
  _selected = None

  def __init__(self, canvas, idx, color):
    """
    Args:
      canvas: The canvas to draw the cube on.
      pos: The initial position of the cube.
      color: The color of the cube. """
    self.__canvas = canvas
    self.__idx = idx

    # Determine position from index
    (x, y) = self.__idx
    x *= Cube.CUBE_SIZE
    y *= Cube.CUBE_SIZE
    x += Cube.CUBE_OFFSET
    y += Cube.CUBE_OFFSET
    self.__pos = (x, y)

    self.__color = color

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

    self.__draw_cube()

  @classmethod
  def get_selected(cls):
    """
    Returns:
      The currently selected cube. """
    return cls._selected

  def __draw_cube(self):
    """ Draws the cube on the canvas. """
    x, y = self.__pos

    # Draw the actual cube shapes.
    base_size = Cube.CUBE_SIZE
    case = obj_canvas.Rectangle(self.__canvas, self.__pos,
                                (base_size, base_size),
                                fill=self.__color, outline=self.__color)
    self.__screen = display.Display(self.__canvas, (x, y - 20), (180, 140))
    button_l = obj_canvas.Rectangle(self.__canvas, (x - 65, y + 75), (50, 30),
                                    fill=colors.BUTTONS, outline=colors.BUTTONS)
    button_c = obj_canvas.Rectangle(self.__canvas, (x, y + 75), (50, 30),
                                    fill=colors.BUTTONS, outline=colors.BUTTONS)
    button_r = obj_canvas.Rectangle(self.__canvas, (x + 65, y + 75), (50, 30),
                                    fill=colors.BUTTONS, outline=colors.BUTTONS)

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
      return

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
    x *= Cube.CUBE_SIZE
    y *= Cube.CUBE_SIZE
    x += Cube.CUBE_OFFSET
    y += Cube.CUBE_OFFSET
    self.set_pos(x, y)
    self.update_connections(others)

  def set_pos(self, x, y):
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

  def send_message(self, side, message):
    """ Sends a message to a cube directly connected to this one.
    Args:
      side: The side that the recipient is connected on.
      message: The string message to send. """
    other = self.__connected[side]
    if other is None:
      # Make sure there's something there.
      raise ValueError("No cube connected on side %s'." % (side))

    # Pass the message to the cube.
    other_side = Cube.Sides.opposite(side)
    other.receive_message(other_side, message)

  def receive_message(self, side, message):
    """ Receives a message from a connected cube.
    Args:
      side: The side that the sender is connected to.
      message: The string message being received. """
    if not self.__application:
      # With no app, the message gets dropped.
      return

    # Otherwise, pass it to the app.
    self.__application.on_message_receive(side, message)

  def snap_to_grid(self, grid_size, others, offset = 0):
    """ Snap this cube to grid.
      Args:
        grid_size: pixel size of grid as (w, h)
        others: List of other cubes the current cube can snap to
        offsest: offset of the snap grid in pixels (x, y) """

    # Snap to proper position
    old_pos = self.get_pos()
    old_idx = self.__idx
    new_x = old_pos[0] - old_pos[0] % grid_size + offset
    new_y = old_pos[1] - old_pos[1] % grid_size + offset

    # makes sure no cube is in the way, moves the cube if so
    (x1, y1) = self.__idx
    x2 = (new_x - Cube.CUBE_OFFSET)//Cube.CUBE_SIZE
    y2 = (new_y - Cube.CUBE_OFFSET)//Cube.CUBE_SIZE
    c = others[y2][x2]
    others[y2][x2] = self
    others[y1][x1] = c

    if c:
        c.__clear_connections()
        c.set_idx(x1, y1, others)

    self.set_idx(x2, y2, others)

  def update_connections(self, others):

    # Check for cubes at each side
    for side in Cube.Sides.all():
      shift = Cube.Sides.coordinates(side)
      other_x, other_y = shift[0] + self.__idx[0], shift[1] + self.__idx[1]
      other = others[other_y][other_x]

      # If a cube exists on this side, add connections
      if other:
          self_changed = self.__add_connection(other, side)
          other_changed = other.__add_connection(self, Cube.Sides.opposite(side))
          if self_changed:
            self.__config_changed_hook()
          if other_changed:
            other.__config_changed_hook()
