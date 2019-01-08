import mock
import os
import subprocess
import unittest

from config import config


class TestConfig(unittest.TestCase):
  """ Tests for the config module. """

  @mock.patch("yaml.load")
  @mock.patch("config.config.file")
  def setUp(self, mocked_file, mocked_load):
    # Delete saved instances.
    config.Config._CONFIG_INSTANCES = {}

    # Create a Config object for testing.
    self.__test_data = {"single": 25, "list": ["hello", "goodbye"],
                        "nested": {"foo": "hello", "baz": 12}}
    # Make it look like it loaded this data.
    mocked_load.return_value = self.__test_data.copy()

    self.__config = config.Config("file.yaml")

  @mock.patch("yaml.load")
  @mock.patch("config.config.file")
  def test_get_config(self, mocked_file, mocked_load):
    """ Tests that get_config works under normal conditions. """
    # Make it look like it loaded the test data.
    mocked_load.return_value = self.__test_data.copy()

    # The fake file object.
    mocked_file_object = mocked_file.return_value
    # The mocked close() method on the file.
    mocked_close = mocked_file_object.close

    # Load a configuration file.
    my_config = config.Config.get_config("config.yaml")

    # It should have opened the file.
    mocked_file.assert_called_once_with("config.yaml")
    # It should have loaded the data.
    mocked_load.assert_called_once_with(mocked_file_object, Loader=mock.ANY)
    # It should have closed the file.
    mocked_close.assert_called_once_with()

    # It should have saved the instance.
    self.assertDictEqual({"config.yaml": my_config},
                         config.Config._CONFIG_INSTANCES)

  @mock.patch("yaml.load")
  @mock.patch("config.config.file")
  def test_get_config_memoized(self, mocked_file, mocked_load):
    """ Tests that get_config works when the file was already loaded. """
    # First, make it look like we already loaded a configuration.
    config.Config._CONFIG_INSTANCES = {"config.yaml": self.__config}

    # Try loading this one again.
    got_config = config.Config.get_config("config.yaml")

    # It should have given us the existing one.
    self.assertEqual(self.__config, got_config)

  def test_get_single(self):
    """ Tests that the get method can get a single value. """
    # Get a single value.
    self.assertEqual(self.__test_data["single"], self.__config.get("single"))

  def test_get_multi_level(self):
    """ Tests that the get method works with lower-level keys. """
    self.assertEqual(self.__test_data["nested"]["foo"],
                     self.__config.get("nested", "foo"))

  def test_get_list(self):
    """ Tests that the get method can get a list. """
    # Get a list.
    got_list = self.__config.get("list")
    self.assertSequenceEqual(self.__test_data["list"], got_list)

    # There should be no rep exposure.
    old_list = got_list[:]
    got_list.append("another")
    self.assertSequenceEqual(old_list, self.__config.get("list"))

  def test_get_nested_dict(self):
    """ Tests that the get method can get an entire nested dictionary. """
    got_dict = self.__config.get("nested")
    self.assertDictEqual(self.__test_data["nested"], got_dict)

    # There should be no rep exposure.
    old_dict = got_dict.copy()
    got_dict["another"] = True
    self.assertDictEqual(old_dict, self.__config.get("nested"))

  def test_get_bad_key(self):
    """ Tests that the get method handles cases where the key is bad. """
    # Try looking for a key that doesn't exist.
    self.assertRaises(KeyError, self.__config.get, "bad_key")
    # Try looking for a key that's too deep.
    self.assertRaises(KeyError, self.__config.get, "list", "key")

  def test_subkeys(self):
    """ Tests that the subkeys method works under normal conditions. """
    self.assertSequenceEqual(self.__test_data["nested"].keys(),
                             self.__config.subkeys("nested"))

  def test_subkeys_bad_key(self):
    """ Tests that the subkeys method handles cases where the key is bad. """
    # Try to get the subkeys of a leaf.
    self.assertRaises(KeyError, self.__config.subkeys, "single")
    self.assertRaises(KeyError, self.__config.subkeys, "list")


if __name__ == "__main__":
  unittest.main()
