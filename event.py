class Event(object):
  """ Represents a GUI event. """

  def __init__(self, tk_event):
    """
    Args:
      tk_event: The underlying Tkinter event to wrap. """
    self._tk_event = tk_event

  @classmethod
  def get_identifier(cls):
    """
    Returns:
      The Tkinter identifier for this event. """
    raise NotImplementedError("Must be implemented by subclass.")

class MouseEvent(Event):
  """ Event involving the mouse. """

  def get_pos(self):
    """
    Returns:
      The position of the mouse during the event, as (x, y). """
    return (self._tk_event.x, self._tk_event.y)

class MouseDragEvent(MouseEvent):
  """ Emitted every time the mouse is dragged with the primary button held down.
  """

  @classmethod
  def get_identifier(cls):
    return "<B1-Motion>"

class MousePressEvent(MouseEvent):
  """ Emitted every time the primary mouse button is pressed. """

  @classmethod
  def get_identifier(cls):
    return "<Button-1>"

class MouseReleaseEvent(MouseEvent):
  """ Emitted every time the primary mouse button is released. """

  @classmethod
  def get_identifier(cls):
    return "<ButtonRelease-1>"
