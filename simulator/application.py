import json


class Application(object):
  """ Represents an application running on a cube. """

  def _start_app(self):
    """ This is the app's start-up code. It will be run once when it is first
    started. """
    return

  def _on_message_receive(self, side, message):
    """ This is called every time the cube receives a message.
    Args:
      side: The side that the sender is connected on.
      message: The message, in deserialized form. """
    return

  def run(self, cube):
    """ Runs the application on a particular cube.
    Args:
      cube: A handle to the cube that this app is running on. """
    self.__cube = cube

    # Run init code.
    self._start_app()

  def on_reconfiguration(self, config):
    """ This is a stub method that will be called every time the connection
    configuration of the cube that this application is running on changes. By
    default, it does nothing.
    Args:
      config: The new configuration, as returned by Cube.get_connections(). """
    return

  def on_message_receive(self, side, message):
    """ This is a stub method that will be called every time a message is
    received by the cube. By default, it does nothing.
    Args:
      side: The side that the sender is connected on.
      message: The message being received, in string form. """
    # Deserialize.
    decoded = json.loads(message)
    self._on_message_receive(side, decoded)

  def send_message(self, side, message):
    """ Sends a message to a connected cube.
    Args:
      side: The side that the recipient is connected on.
      message: The message to send. Can be anything JSONable. """
    # Serialize.
    serialized = json.dumps(message)
    # Send it.
    self.__cube.send_message(side, serialized)

  def draw_text(self, *args, **kwargs):
    """ Draws text on the cube screen. Arguments are passed transparently to the
    cube display object. """
    display = self.__cube.get_display()
    return display.draw_text(*args, **kwargs)

  def clear_display(self):
    """ Clears the cube screen. """
    display = self.__cube.get_display()
    return display.clear()

  def set_background_color(self, color):
    """ Sets the display background color.
    Args:
      color: The color to set it to. """
    display = self.__cube.get_display()
    display.set_fill(color)
