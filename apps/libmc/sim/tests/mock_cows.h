#ifndef LIBMC_SIM_TESTS_MOCK_COWS_H_
#define LIBMC_SIM_TESTS_MOCK_COWS_H_

#include <stdint.h>

#include "gmock/gmock.h"

#include "apps/libmc/sim/cows_interface.h"

namespace libmc {
namespace sim {
namespace testing {

// Mock class for Cows.
class MockCows : public CowsInterface {
 public:
  MOCK_METHOD2(CowsStuff, void(uint16_t *buffer, uint16_t length));
  MOCK_METHOD2(CowsUnstuff, void(uint16_t *buffer, uint16_t length));
};

}  // namespace testing
}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_TESTS_MOCK_COWS_H_
