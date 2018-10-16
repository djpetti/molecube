#include "cows.h"

#include <assert.h>

namespace libmc {
namespace sim {

void CowsStuff(uint16_t *buffer, uint16_t length) {
  assert(length >= 2 && "Length must be at least 2.");

  uint16_t *const data_end = buffer + length;
  uint16_t *last_zero = data_end;

  // Make a single pass backwards to remove all the zeroes.
  // Ignore the first two words, which will be filled in by us later.
  for (uint16_t *word = data_end - 1; word >= buffer + 2; --word) {
    if (!(*word)) {
      // Replace the zero.
      *word = last_zero - word;
      last_zero = word;
    }
  }

  // Fill in the overhead byte and packet separator.
  buffer[0] = 0;
  buffer[1] = last_zero - (buffer + 1);
}

void CowsUnstuff(uint16_t *buffer, uint16_t length) {
  assert(length >= 2 && "Length must be at least 2.");
  assert(!buffer[0] && "Got invalid packet separator.");

  // Location of first zero is notated in word 1.
  uint16_t *next_zero = buffer + 1 + buffer[1];

  // Make a single forward pass to replace all the zeroes.
  while (next_zero < buffer + length) {
    const uint16_t move_forward = *next_zero;
    *next_zero = 0;
    next_zero += move_forward;
  }
}

}  // namespace sim
}  // namespace libmc
