import os

import yaml
try:
  # Use accelerated versions if we have them.
  from yaml import CLoader as Loader
except ImportError:
  from yaml import Loader


# The path to the config directory.
_G_CONFIG_PATH = os.path.dirname(os.path.abspath(__file__))


def cube_config():
  """ Shortcut function to get the Config instance for the cube.yaml
  configuration file.
  Returns:
    The Config instance. """
  cube_file = os.path.join(_G_CONFIG_PATH, "cube.yaml")
  return Config.get_config(cube_file)

def simulator_config():
  """ Shortcut function to get the Config instance for the simulator.yaml
  configuration file.
  Returns:
    The Config instance. """
  sim_file = os.path.join(_G_CONFIG_PATH, "simulator.yaml")
  return Config.get_config(sim_file)


class Config(object):
  """ Thin wrapper around YAML config files that allows for easy attribute
  access. """

  # Singleton configuration instances, one for each unique file.
  _CONFIG_INSTANCES = {}

  @classmethod
  def get_config(cls, filename):
    """ Gets a configuration instance. It will use an existing one if it can,
    and create a new one if it has to. This should always be used by the public
    as opposed to instantiating the class directly.
    Args:
      filename: The name of the configuration file.
    Returns:
      The Config instance. """
    if filename in cls._CONFIG_INSTANCES:
      # We already have it.
      return cls._CONFIG_INSTANCES[filename]

    # Create and save a new one.
    instance = cls(filename)
    cls._CONFIG_INSTANCES[filename] = instance

    return instance

  def __init__(self, config_file):
    """
    Args:
      config_file: The config file to read data from. """
    # Load the config data.
    self.__data = self.__load_config(config_file)

  def __load_config(self, config_file):
    """ Loads the actual data from the file.
    Args:
      config_file: The file to load data from.
    Returns:
      The dictionary of loaded data. """
    open_config = file(config_file)
    data = yaml.load(open_config, Loader=Loader)
    open_config.close()

    return data

  def get(self, *args):
    """ Gets a configuration value.
    Args:
      The arguments are interpreted, in order, as the keys for the specified
      item. """
    item = self.__data
    for key in args:
      if type(item) is not dict:
        raise KeyError("Too many keys; cannot index %s with '%s'." % \
            (str(item), key))
      if key not in item:
        raise KeyError("Invalid key '%s'." % (key))

      # Go to the next level.
      item = item[key]

    # If it's an internal container, avoid rep exposure by copying.
    if hasattr(item, "copy"):
      return item.copy()
    elif hasattr(item, "__getitem__"):
      return item[:]
    return item

  def subkeys(self, *args):
    """ Gets a list of subkeys for a particular key.
    Args:
      The arguments are interpreted, in order, as the keys for the specified
      level. """# Singleton configuration instances, one for each unique file.
    level = self.get(*args)

    if type(level) is not dict:
      # It should be a leaf.
      raise KeyError("Cannot get list of subkeys for single value.")

    # Get the subkeys.
    return level.keys()
