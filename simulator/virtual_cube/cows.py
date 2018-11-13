import ctypes


def _pad_array(array):
  """ Pad array if necessary to make it word-aligned.
  Args:
    array: The array to pad.
  Returns:
    True iff the array was changed. """
  if len(array) % 2:
    # We need to pad. We'll pad with ones to make sure we don't add an
    # artificial zero.
    array.append(1)
    return True

  return False

def _unpad_array(array):
  """ Unpad array if it was padded at the beginning.
  Args:
    array: The array to unpad. """
  array.pop()

def _set_word(array, index, value):
  """ Shorthand for setting two byte values in an array to a word value.
  Args:
    array: The array to set stuff in.
    index: The start index of the word to set.
    value: A c_ushort value that we want to set there. """
  array[index] = value.value >> 8
  array[index + 1] = value.value & 0x00FF

def _get_word(array, index):
  """ Shorthand for getting two byte values in an array.
  Args:
    array: The array the get stuff from.
    index: The start index of the word to get.
  Returns:
    A c_ushort containing the value of the word. """
  value = ctypes.c_ushort(array[index] << 8)
  value.value |= array[index + 1]

  return value

def cows_stuff(array):
  """ Performs COWS stuffing on an input.
  Args:
    array: The input to stuff. The first word will
           be used for the overhead, and so should not contain any necessary
           data. """
  padded = _pad_array(array)

  last_zero = len(array) / 2

  # Make a single pass backwards to remove all the zeroes.
  for i in range(len(array) - 1, 2, -2):
    byte2 = array[i]
    byte1 = array[i - 1]

    if (byte1 == 0 and byte2 == 0):
      # Replace the zero.
      to_next = last_zero - i / 2
      last_zero -= to_next
      to_next = ctypes.c_ushort(to_next)

      _set_word(array, i - 1, to_next)

  # Fill in the overhead word.
  overhead = ctypes.c_ushort(last_zero)
  _set_word(array, 0, overhead)

  if padded:
    # Remove the padding.
    _unpad_array(array)

def cows_unstuff(array):
  """ Performs the COWS unstuffing on an input.
  Args:
    array: The input to unstuff. The first word will be assumed to be the
           overhead. """
  padded = _pad_array(array)

  # Make a single forward pass to replace all the zeroes.
  i = 0
  while i < len(array):
    move_forward = _get_word(array, i)

    # Set the zero.
    _set_word(array, i, ctypes.c_ushort(0))

    # Move to the next zero.
    i += 2 * move_forward.value

  if padded:
    # Remove the padding.
    _unpad_array(array)
