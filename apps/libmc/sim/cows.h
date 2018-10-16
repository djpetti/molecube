#ifndef MYELIN_NET_COWS_H_
#define MYELIN_NET_COWS_H_

#include <stdint.h>

// Defines an implementation of a variation of Consistent Overhead Byte Stuffing
// (COBS) which operates on the basis of words (2-byte) instead of bytes. This
// allows for lower overhead when stuffing long messages.

namespace libmc {
namespace sim {

// Stuffs a buffer in-place. It will automatically add the 0 packet divider and
// overhead word at the beginning of the buffer, so these first two words should
// not contain actual data.
// Args:
//  buffer: The buffer to stuff.
//  length: The usable length of the buffer. Note that the COWS implentation
//          reserves the two words at the start of the buffer, so length should
//          be at least 2.
void CowsStuff(uint16_t *buffer, uint16_t length);

// Un-stuffs a buffer in-place. It expects the first two words to be the 0
// packet divider and overhead word. These will consequently be unchanged in the
// unstuffed version.
// Args:
//  buffer: The buffer stuffed with CowsStuff() to be un-stuffed.
//  length: The usable length of the buffer. Note that the two bytes at the
//          beginning of the buffer will not be changed, so length should be at
//          least 2.
void CowsUnstuff(uint16_t *buffer, uint16_t length);

}  // namespace sim
}  // namespace libmc

#endif  // MYELIN_NET_COWS_H_
