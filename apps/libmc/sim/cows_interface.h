#ifndef LIBMC_SIM_COWS_INTERFACE_H_
#define LIBMC_SIM_COWS_INTERFACE_H_

#include <stdint.h>

namespace libmc {
namespace sim {

// Common interface for COWS.
class CowsInterface {
 public:
  virtual ~CowsInterface() = default;

  // Stuffs a buffer in-place. It will automatically add the overhead word at
  // the beginning of the buffer, so this first word should not contain
  // actual data.
  // Args:
  //  buffer: The buffer to stuff.
  //  length: The usable length of the buffer. Note that the COWS implentation
  //          reserves one word at the start of the buffer, so length should
  //          be at least 1.
  virtual void CowsStuff(uint16_t *buffer, uint16_t length) = 0;

  // Un-stuffs a buffer in-place. It expects the first word to be the overhead
  // word. This will consequently be unchanged in the unstuffed version.
  // Args:
  //  buffer: The buffer stuffed with CowsStuff() to be un-stuffed.
  //  length: The usable length of the buffer. Note that the one word at the
  //          beginning of the buffer will not be changed, so length should be at
  //          least 1.
  virtual void CowsUnstuff(uint16_t *buffer, uint16_t length) = 0;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_COWS_INTERFACE_H_
