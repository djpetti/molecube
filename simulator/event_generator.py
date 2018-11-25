import logging

import event


logger = logging.getLogger(__name__)


class _EventGenerator(object):
  """ Base class for custom Tkinter event generators. """

  def __init__(self, canvas, event_class):
    """
    Args:
      canvas: The Canvas to generate events for.
      event_class: Event subclass that this generator emits. """
    self.__canvas = canvas
    self.__event_name = event_class.get_identifier()

  def _dispatch(self, cube_id, **kwargs):
    """ Dispatches a new event to the Tkinter loop.
    Args:
      cube_id: The ID of the cube that this message came from.
      Keyword arguments will be used as the event parameters. """
    # Add the cube ID as an event parameter.
    event_params = kwargs
    event_params["cube_id"] = cube_id

    logger.debug("Dispatching %s event: %s" % (self.__event_name,
                                               str(event_params)))
    self.__canvas.generate_event(self.__event_name, **event_params)

  def dispatch(self, cube_id, message):
    """ Dispatches an event to the Tkinter loop.
    Args:
      cube_id: The ID of the cube that this message came from.
      message: The SimMessage to base the event on. """
    raise NotImplementedError("dispatch() must be implemented by subclass.")


class GraphicsEventGenerator(_EventGenerator):
  """ Generates graphics events. """

  def __init__(self, canvas):
    """
    Args:
      canvas: The Canvas to generate events for. """
    super(GraphicsEventGenerator, self).__init__(canvas, event.GraphicsEvent)

  def dispatch(self, cube_id, message):
    # Extract the graphics message.
    graphics_message = message.graphics

    op_type = graphics_message.op_type
    image_width = graphics_message.image.width
    image_height = graphics_message.image.height
    image_data = graphics_message.image.data

    self._dispatch(cube_id, op_type=op_type, image_width=image_width,
                   image_height=image_height, image_data=image_data)
