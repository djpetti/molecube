import ConfigParser


""" Wrapper around configuration file so we don't have to change the filename in
a bunch of places. """

# Name of the configuration file.
_CONFIG_FILE = "simulator/config.ini"

# Parse the config file.
_parser = ConfigParser.ConfigParser()
if not _parser.read(_CONFIG_FILE):
  raise ValueError("Could not open config file '%s'." % (_CONFIG_FILE))

def get(section, attribute):
  """ Gets the value of an attribute from the config file.
  Args:
    section: The section to read from.
    attribute: The attribute to read in that section.
  Returns:
    The value of the attribute. """
  return _parser.get(section, attribute)

def items(section):
  """ Gets a list of the attributes in a section.
  Args:
    section: The section name.
  Returns:
    The attributes in the section. """
  return _parser.items(section)
