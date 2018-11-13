import mock
import os
import subprocess
import unittest

from simulator.virtual_cube import cows


class TestCows(unittest.TestCase):
  """ Tests for the COWS functions. """

  def __check_zero_free(self, array):
    """ Checks that an array has no zeros in it.
    Args:
      array: The array to check. """
    for i in range(0, len(array), 2):
      elem = array[i:i + 2]
      self.assertNotEqual(bytearray([0, 0]), elem)

  def __create_buffer(self, length):
    """ Creates a "random" buffer for testing.
    Args:
      The length of the buffer to create.
    Returns:
      The buffer it created. """
    array = bytearray(length)
    array[0] = 1
    for i in range(1, length):
      # Modulo a prime creates something that looks random-ish.
      array[i] = 3 * array[i - 1] % 19

    return array

  def test_cows(self):
    """ Tests that COWS works under normal conditions. """
    array = self.__create_buffer(1024)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])

  def test_cows_padded(self):
    """ Tests that COWS works on a buffer that gets padded. """
    array = self.__create_buffer(1023)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])

  def test_cows_all_zeros(self):
    """ Tests that COWS works on a buffer that's all zeros. """
    array = bytearray([0, 0] * 512)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # The entire array should be filled with ones.
    expected = bytearray([0, 1] * 512)
    self.assertSequenceEqual(expected, array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])

  def test_cows_padded_all_zeros(self):
    """ Tests that COWS works on a zero buffer that gets padded. """
    array = bytearray([0] * 1023)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # The entire array should be filled with ones, except for the last word.
    # This should be 2, because we padded with a non-zero byte.
    expected = bytearray([0, 1] * 510) + bytearray([0, 2, 0])
    self.assertSequenceEqual(expected, array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])

  def test_cows_no_zeros(self):
    """ Tests that COWS works on a buffer that has no zeros. """
    array = bytearray([1, 1] * 512)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # The array should be unchanged, aside from the overhead.
    expected = bytearray([2, 0]) + original[2:]
    self.assertSequenceEqual(expected, array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])

  def test_cows_padded_no_zeros(self):
    """ Tests that COWS works on a buffer with no zeros that gets padded. """
    array = bytearray([1] * 1023)
    original = array[:]

    # Stuff the array.
    cows.cows_stuff(array)
    self.__check_zero_free(array)

    # The array should be unchanged, aside from the overhead.
    expected = bytearray([2, 0]) + original[2:]
    self.assertSequenceEqual(expected, array)

    # Now try unstuffing it.
    cows.cows_unstuff(array)
    # We should get the original back, except for the overhead, which we don't
    # care about.
    self.assertSequenceEqual(original[2:], array[2:])


if __name__ == "__main__":
  unittest.main()
