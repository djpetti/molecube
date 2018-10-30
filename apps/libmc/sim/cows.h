#ifndef LIBMC_SIM_COWS_H_
#define LIBMC_SIM_COWS_H_

#include <stdint.h>

#include "cows_interface.h"

namespace libmc {
namespace sim {

// Defines an implementation of a variation of Consistent Overhead Byte Stuffing
// (COBS) which operates on the basis of words (2-byte) instead of bytes. This
// allows for lower overhead when stuffing long messages.
class Cows : public CowsInterface {
 public:
  virtual void CowsStuff(uint16_t *buffer, uint16_t length);
  virtual void CowsUnstuff(uint16_t *buffer, uint16_t length);
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_COWS_H_
