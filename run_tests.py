#!/usr/bin/python


import os
import unittest


""" Script to run the Python tests. """


def run_python_tests():
  """ Runs the Python tests.
  Returns:
    True if the tests all succeed, False if there are failures. """
  print "Starting tests..."

  loader = unittest.TestLoader()
  # Get the directory this module is in.
  dir_path = os.path.dirname(os.path.realpath(__file__))
  suite = loader.discover("simulator", top_level_dir=dir_path)

  test_result = unittest.TextTestRunner(verbosity=2).run(suite)
  if not test_result.wasSuccessful():
    return False

  return True

if __name__ == "__main__":
  run_python_tests()
