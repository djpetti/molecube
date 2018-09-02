import event
import obj_canvas


class Cube(object):
  """ Represents a single cube. """

  def __init__(self, canvas, pos):
    """
    Args:
      canvas: The canvas to draw the cube on.
      pos: The initial position of the cube. """
    self.__canvas = canvas
    # Whether the cube is currently being dragged.
    self.__dragging = False
    # List of shapes in the cube.
    self.__cube_shapes = []

    self.__draw_cube()

  def __draw_cube(self):
    """ Draws the cube on the canvas. """
    # Draw the actual cube shapes.
    case = obj_canvas.Rectangle(self.__canvas, (100, 100), (200, 200),
                                fill="red")

    self.__cube_shapes.append(case)

    # Bind mouse events for the cube.
    case.bind_event(event.MousePressEvent, self.__cube_clicked)
    case.bind_event(event.MouseDragEvent, self.__cube_dragged)

  def __cube_clicked(self, event):
    """ Called when the user presses the mouse button over the cube. """
    # We are now dragging this cube.
    self.__dragging = True

    # When moving, keeps track of the previous mouse position.
    self.__prev_mouse_x, self.__prev_mouse_y = event.get_pos()

  def __cube_dragged(self, event):
    """ Called when the user moves the mouse over the cube while the button is
    pressed. """
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
    self.__dragging = False


class Tabletop(object):
  """ Simulates a "tabletop" in which the cubes exist. """

  def __init__(self):
    # List of cubes.
    self.__cubes = []

    # Canvas on which to draw cubes.
    self.__canvas = obj_canvas.Canvas()

    # When we release the mouse button, we want to clear the dragging state for
    # all the cubes.
    self.__canvas.bind_event(event.MouseReleaseEvent, self.__mouse_released)

  def __mouse_released(self, event):
    """ Called when the user releases the mouse button. """
    # Clear the dragging state for all the cubes.
    for cube in self.__cubes:
      cube.clear_drag()

  def make_cube(self):
    """ Adds a new cube to the canvas. """
    cube = Cube(self.__canvas, (100, 100))
    self.__cubes.append(cube)

  def run(self):
    """ Runs the tabletop simulation indefinitely. """
    self.__canvas.wait_for_events()
