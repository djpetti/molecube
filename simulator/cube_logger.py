import logging

from config import config


# Config class for the simulator.
sim_config = config.simulator_config()


class CubeLogger(logging.Logger):
  """ Logger for the simulator. """

  def __init__(self, name):
    """
    Args:
      name: The name of the logger. """
    super(CubeLogger, self).__init__(name)

    # Set the root logging level.
    self.setLevel(logging.DEBUG)

    log_location = config.get('logging', 'log_file')

    # Create the Handler for logging data to a file
    file_handler = logging.FileHandler(log_location, mode="w")
    file_handler.setLevel(logging.DEBUG)
    # Create the Handler for logging important messages to stdout.
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.INFO)

    # Create a Formatter for formatting the log messages
    logger_formatter = logging.Formatter('%(name)s - %(levelname)s - %(message)s')

    # Add the Formatter to the handlers.
    file_handler.setFormatter(logger_formatter)
    stream_handler.setFormatter(logger_formatter)

    # Add the handlers to the Logger
    self.addHandler(file_handler)
    self.addHandler(stream_handler)


logging.setLoggerClass(CubeLogger)
