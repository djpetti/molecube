import logging

import cube
import event_generator


logger = logging.getLogger(__name__)


class CubeEventHandler(object):
  """ Contains all the functionality for handling events from cubes. """

  def __init__(self, canvas):
    """
    Args:
      canvas: The canvas to generate events on. """
    self.__init_event_generators(canvas)

  def __init_event_generators(self, canvas):
    """ Initialize generators for all types of events.
    Args:
      canvas: The canvas to generate events on. """
    self.__event_generators = {}

    self.__event_generators["graphics"] = \
        event_generator.GraphicsEventGenerator(canvas)

  def __get_next_cube_messages(self, cubes):
    """ Waits until at least one cube has a message available, and reads it.
    Args:
      cubes: The cubes to get messages from.
    Returns:
      A dict mapping cubes to messages that were read. """
    # Wait for a readable message.
    readable = cube.Cube.select_on(cubes)

    # Read all available messages.
    messages = {}
    for my_cube in readable:
      message = my_cube.receive_message()
      messages[my_cube] = message

      cube_id = my_cube.get_id()
      logger.debug("Got message from cube %d: %s" % (cube_id, message))

    return messages

  def __dispatch_events(self, cube, message):
    """ Dispatches the proper events based on a message that was received.
    Args:
      cube: The cube that the message came from.
      message: The received message. """
    # Go through the sub-messages and dispatch the appropriate events.
    for message_name, generator in self.__event_generators.iteritems():
      if message.HasMessage(message_name):
        # We need to dispatch an event for this.
        generator.dispatch(cube, message)

  def handle_events(self, cubes):
    """ Waits for events originating from the cubes, and sent over the serial
    interface. When it receives a message, it dispatches a custom Tkinter event
    which can then be handled in the main thread. This function will run a
    single waiting cycle and then return.
    Args:
      cubes: The list of cubes to listen for events on. """
    # Wait for an event from some cube.
    logger.info("Waiting for cube events...")
    messages = self.__get_next_cube_messages(cubes)

    # Dispatch the appropriate events.
    for cube, message in messages.iteritems():
      self.__dispatch_events(cube, message)
