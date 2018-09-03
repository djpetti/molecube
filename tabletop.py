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

    # Associates each side with its opposite.
    _OPPOSITES = {LEFT: RIGHT,
                  RIGHT: LEFT,
                  TOP: BOTTOM,
                  BOTTOM: TOP}

    @classmethod
    def opposite(cls, side):
      """ Gets the opposite side to a specified one.
      Args:
        side: The side to get the opposite of.
      Returns:
        The opposite side. """
      return cls._OPPOSITES[side]

  # Base cube size, in px.
  CUBE_SIZE = 200

  # Currently selected cube. There can be only one.
  _selected = None

  def __init__(self, canvas, pos, color):
    """
    Args:
      canvas: The canvas to draw the cube on.
      pos: The initial position of the cube.
      color: The color of the cube. """
    self.__canvas = canvas
    self.__pos = pos
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
      side: The side at which this cube is connected. """
    if self.__connected[side] == other:
      return

    self.__connected[side] = other
    # Report a state change.
    self.__config_changed_hook()

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

  def set_pos(self, x, y):
    """ Sets the position of the cube.
    Args:
      x: The new x position.
      y: The new y position. """
    # Figure out the offset from the old position.
    old_x, old_y = self.get_pos()
    move_x = x - old_x
    move_y = y - old_y

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

  def is_near(self, other, threshold=100):
    """ Checks if this cube is near another one.
    Args:
      other: The cube to check if we are near.
      threshold: Boundary at which we consider ourselves near.
    Returns:
      None if the two cubes are not near, otherwise it returns the side of this
      cube that is nearest the other cube. """
    # We'll use the bounding boxes on the case rectangles for this check, since
    # they are the biggest.
    my_case = self.__cube_shapes[0]
    other_case = other.__cube_shapes[0]

    col_x, col_y = obj_canvas.CanvasObject.check_collision(my_case, other_case,
                                                           threshold=threshold)

    if (not col_x or not col_y):
      # No collision.
      return None

    my_case_x, my_case_y = my_case.get_pos()
    other_case_x, other_case_y = other_case.get_pos()
    x_dist = abs(my_case_x - other_case_x)
    y_dist = abs(my_case_y - other_case_y)

    if y_dist < x_dist:
      if my_case_x > other_case_x:
        # Our left side is colliding.
        return Cube.Sides.LEFT
      else:
        # Our right side is colliding.
        return Cube.Sides.RIGHT

    else:
      if my_case_y > other_case_y:
        # Our top side is colliding.
        return Cube.Sides.TOP
      else:
        # Our bottom side is colliding.
        return Cube.Sides.BOTTOM

  def snap(self, other, side):
    """ Snap this cube to another cube.
    Args:
      other: The cube to snap to.
      side: The side of this cube to snap on. """
    # Align the cubes in the display.
    other_x, other_y = other.get_pos()
    new_x = None
    new_y = None

    if side in (Cube.Sides.LEFT, Cube.Sides.RIGHT):
      # We need to align the y-axis.
      new_y = other.get_pos()[1]
    else:
      # We need to align the x-axis.
      new_x = other.get_pos()[0]

    # Make sure the sides are touching.
    size = Cube.CUBE_SIZE
    if side == Cube.Sides.LEFT:
      new_x = other_x + size
    elif side == Cube.Sides.RIGHT:
      new_x = other_x - size
    elif side == Cube.Sides.TOP:
      new_y = other_y + size
    else:
      new_y = other_y - size

    self.set_pos(new_x, new_y)

    # Indicate that we are connected to this cube.
    self.__add_connection(other, side)
    other.__add_connection(self, Cube.Sides.opposite(side))


class Tabletop(object):
  """ Simulates a "tabletop" in which the cubes exist. """

  def __init__(self):
    # List of cubes.
    self.__cubes = []

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

    # Check if the cube is near any others.
    for cube in self.__cubes:
      if cube == selected_cube:
        # No point in checking ourselves.
        continue

      near_side = selected_cube.is_near(cube)
      if near_side is not None:
        # We are near this cube. Snap to it.
        selected_cube.snap(cube, near_side)
        break

    selected_cube.clear_drag()

  def __mouse_dragged(self, event):
    """ Called when the user drags with the mouse. """
    # Get the currently-selected cube.
    selected_cube = Cube.get_selected()
    if selected_cube is None:
      # No cube is selected. Do nothing.
      return

    # Move the cube.
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
