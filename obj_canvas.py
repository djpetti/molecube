import Tkinter as tk

import event


class GuiObject(object):
  """ A high-level class that spans all objects used in the GUI. """

  class Callback(object):
    """ A special class that wraps a callback. Whenever it is called, it will
    wrap the Tkinter event in an Event object and pass it on. """

    def __init__(self, event_type, callback):
      """
      Args:
        event_type: The event class to wrap with.
        callback: The callback function. """
      self.__event_type = event_type
      self.__callback = callback

    def __call__(self, tk_event):
      """
      Args:
        tk_event: The Tkinter event. """
      # Wrap the event.
      event = self.__event_type(tk_event)
      # Call the function.
      self.__callback(event)

  def _do_event_bind(self, event_name, callback):
    """ Performs the actual event binding. This should be implemented by
    subclasses.
    Args:
      event_name: The name of the Tkinter event to bind.
      callback: The callback to run. """
    raise NotImplementedError( \
        "_do_event_bind() must be implemented by subclass.")

  def bind_event(self, event_type, callback):
    """ Binds an event to this object.
    Args:
      event_type: The event class.
      callback: The callback to run when the event happens. It will be passed a
                single instance of Event as an argument. """
    # Create the wrapped callback.
    wrapped = GuiObject.Callback(event_type, callback)
    # Get the Tkinter name of the event.
    tk_name = event_type.get_identifier()
    # Do the binding.
    self._do_event_bind(tk_name, wrapped)


class Canvas(GuiObject):
  """ Simple wrapper around Tkinter canvas. """

  def __init__(self, window_width=None, window_height=None):
    """
    Args:
      window_width: The width of the window.
      window_height: The height of the window. """
    # A dictionary keyed by event types. For each event type, there is a list of
    # tuples containing children and their corresponding callbacks. This is used
    # to determine when we should dispatch an event to a child object.
    self.__child_dispatches = {}

    self.__window = tk.Tk()

    self.__window_width = window_width
    self.__window_height = window_height
    if window_width is None:
      # Use the full screen width.
      self.__window_width = self.__window.winfo_screenwidth()
    if window_height is None:
      # Use the full screen height.
      self.__window_height = self.__window.winfo_screenheight()

    self.__canvas = tk.Canvas(self.__window, width=self.__window_width,
                              height=self.__window_height)
    self.__canvas.pack()

    self.update()

  def __intercept_child_event(self, event):
    """ Intercepts and dispatches events that are intended for child objects.
    Args:
      event: The event that we intercepted. """
    # First, find the proper child list.
    event_type = event.__class__
    assert event_type in self.__child_dispatches
    child_list = self.__child_dispatches[event_type]

    # Check to see if we clicked within any objects.
    click_point = event.get_pos()
    for child, callback in child_list:
      if child.point_within(click_point):
        # We clicked on it. Run the callback.
        callback(event)
        break

  def _do_event_bind(self, event_name, callback):
    # We can directly bind to the Tkinter canvas.
    self.__canvas.bind(event_name, callback)

  def bind_to_child(self, child, event_type, callback):
    """ Certain events can be bound to children of the canvas, specifically,
    mouse events. This method is used by the child object to specify such a
    binding. When this event is triggered, the canvas will determine if the
    mouse was on top of the object, and forward the event accordingly.
    Args:
      child: The child object.
      event_type: The event type to bind.
      callback: The callback to be run. """
    if not issubclass(event_type, event.MouseEvent):
      raise TypeError("Only a MouseEvent can be bound to a child.")

    # The way we'll implement this is by binding to ourselves and intercepting
    # dispatched events.
    if event_type not in self.__child_dispatches:
      # First registration of this event type.
      self.__child_dispatches[event_type] = []
    self.__child_dispatches[event_type].append((child, callback))

    # Intercept this event when it happens.
    self.bind_event(event_type, self.__intercept_child_event)

  def update(self):
    """ Updates the canvas. """
    self.__window.update()

  def wait_for_events(self):
    """ Runs the event loop forever. """
    self.__window.mainloop()

  def move_object(self, *args, **kwargs):
    """ Shortcut for moving an object on the underlying canvas. The arguments
    are passed transparently to canvas.move. """
    self.__canvas.move(*args, **kwargs)

  def delete_object(self, *args, **kwargs):
    """ Shortcut for deleting an object from the underlying canvas. The
    arguments are passed transparently to canvas.delete. """
    self.__canvas.delete(*args, **kwargs)

  def get_raw_canvas(self):
    """ Returns: The underlying Tk canvas object. """
    return self.__canvas

  def get_window_size(self):
    """ Returns: The window width and height, as a tuple. """
    return (self.__window_width, self.__window_height)

  def set_background_color(self, color):
    """ Sets the background color of the canvas.
    Args:
      color: The color to set it to. """
    self.__canvas.config(bg=color)


class CanvasObject(GuiObject):
  """ Handles drawing an object in a Tkinter canvas window. """

  def __init__(self, canvas, pos, fill=None, outline="black"):
    """
    Args:
      canvas: The Canvas to draw on.
      pos: The center position of the object.
      fill: The fill color of the object. """
    self._canvas = canvas

    # Keeps track of the reference for this object.
    self._reference = None
    # Keeps track of the object's center position.
    self._pos_x, self._pos_y = pos
    # The object's fill color.
    self._fill = fill
    # The object's outline color.
    self._outline = outline

    self.__draw_object()

  def __draw_object(self):
    """ Wrapper for _draw_object that deletes an existing object first. """
    if self._reference:
      self.delete()

    self._draw_object()

  def _draw_object(self):
    """ Draws the object on the canvas. Should be implemented by the user. After
    this is called, someone still manually has to call canvas.update() to
    display it. It should also set _reference to the reference of the underlying
    canvas object, and set _pos_x and _pos_y accordingly. """
    raise NotImplementedError("_draw_object() must be implemented by subclass.")

  def _get_bbox(self):
    """ Gets the bounding box for this object. Must be implemented by the
    subclass.
    Returns:
      The bounding box, as a tuple containing the upper left corner coordinates,
      and the lower right corner coordinates. """
    raise NotImplementedError("_get_bbox() must be implemented by subclass.")

  def point_within(self, point):
    """ Checks if a point is within an object. By default, this works by
    checking if the point is within the bounding box.
    Args:
      point: The point to check.
    Returns:
      True if the point is within the object, false otherwise. """
    x, y = point

    # Get the bounding box.
    pt1_x, pt1_y, pt2_x, pt2_y = self._get_bbox()

    in_x = (x >= pt1_x and x <= pt2_x)
    in_y = (y >= pt1_y and y <= pt2_y)
    if (in_x and in_y):
      # Point is within.
      return True

    return False

  def set_pos(self, new_x, new_y):
    """ Moves the object to a new position.
    Args:
      new_x: The new x position.
      new_y: The new y position. """
    move_x = new_x - self._pos_x
    move_y = new_y - self._pos_y

    # Update the position.
    self._pos_x = new_x
    self._pos_y = new_y

    self._canvas.move_object(self._reference, move_x, move_y)
    self._canvas.update()

  def get_pos(self):
    """
    Returns:
      The position of the object. """
    return (self._pos_x, self._pos_y)

  def move(self, x_shift, y_shift):
    """ Moves an object by a certain amount. It does not update the canvas
    afterwards.
    Args:
      x_shift: How far to move in the x direction.
      y_shift: How far to move in the y direction. """
    # Update the position.
    self._pos_x += x_shift
    self._pos_y += y_shift

    self._canvas.move_object(self._reference, x_shift, y_shift)

  def delete(self):
    """ Deletes the object from the canvas. """
    self._canvas.delete_object(self._reference)

    # Indicates that the object is not present.
    self._reference = None

  def bind_event(self, event_type, callback):
    # Delegate the binding to the canvas.
    self._canvas.bind_to_child(self, event_type, callback)

  @classmethod
  def check_collision(cls, obj1, obj2):
    """ Checks if there is a collision between two objects.
    Args:
      obj1: The first object.
      obj2: The second object.
    Returns:
      A tuple of booleans. The first element indicates whether there is a
      collision in the x direction, the second indicates whether there is a
      collision in the y direction. """
    # Get the bounding boxes of both objects.
    pt1_x, pt1_y, pt2_x, pt2_y = obj1._get_bbox()
    pt3_x, pt3_y, pt4_x, pt4_y = obj2._get_bbox()

    # Check for overlap.
    half_width1 = (pt2_x - pt1_x) / 2
    half_width2 = (pt4_x - pt3_x) / 2
    half_height1 = (pt2_y - pt1_y) / 2
    half_height2 = (pt4_y - pt3_y) / 2

    center1_x = pt1_x + half_width1
    center2_x = pt3_x + half_width2
    center1_y = pt1_y + half_height1
    center2_y = pt3_y + half_height2

    center_x_dist = abs(center2_x - center1_x)
    center_y_dist = abs(center2_y - center1_y)

    collision = [False, False]
    if center_x_dist <= half_width1 + half_width2:
      collision[0] = True
    if center_y_dist <= half_height1 + half_height2:
      collision[1] = True

    return collision

class Circle(CanvasObject):
  """ Draws a circle on the canvas. """

  def __init__(self, canvas, pos, radius, **kwargs):
    """
    Args:
      canvas: The Canvas to draw on.
      pos: The center position of the circle.
      radius: The radius of the circle. """
    self.__radius = radius

    super(Circle, self).__init__(canvas, pos, **kwargs)

  def _draw_object(self):
    """ Draw the circle on the screen. """
    # Get the raw canvas to draw with.
    canvas = self._canvas.get_raw_canvas()

    p1_x, p1_y, p2_x, p2_y = self._get_bbox()
    self._reference = canvas.create_oval(p1_x, p1_y, p2_x, p2_y,
                                         fill=self._fill,
                                         outline=self._outline)

  def _get_bbox(self):
    """ See superclass documentation. """
    # Calculate corner points.
    p1_x = self._pos_x - self.__radius
    p1_y = self._pos_y - self.__radius
    p2_x = self._pos_x + self.__radius
    p2_y = self._pos_y + self.__radius

    return (p1_x, p1_y, p2_x, p2_y)

class Rectangle(CanvasObject):
  """ Draws a rectangle on the canvas. """

  def __init__(self, canvas, pos, size, **kwargs):
    """
    Args:
      canvas: The Canvas to draw on.
      pos: The center position of the rectangle.
      size: The width and height of the rectangle. """
    self.__width, self.__height = size

    super(Rectangle, self).__init__(canvas, pos, **kwargs)

  def _draw_object(self):
    """ Draw the rectangle on the canvas. """
    # Get the raw canvas to draw with.
    canvas = self._canvas.get_raw_canvas()

    p1_x, p1_y, p2_x, p2_y = self._get_bbox()
    self._reference = canvas.create_rectangle(p1_x, p1_y, p2_x, p2_y,
                                              fill=self._fill,
                                              outline=self._outline)

  def _get_bbox(self):
    # Calculate corner points.
    p1_x = self._pos_x - self.__width / 2
    p1_y = self._pos_y - self.__height / 2
    p2_x = self._pos_x + self.__width / 2
    p2_y = self._pos_y + self.__height / 2

    return (p1_x, p1_y, p2_x, p2_y)
