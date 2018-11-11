#ifndef LIBMC_SIM_TESTS_MOCK_SIMULATOR_COM_H_
#define LIBMC_SIM_TESTS_MOCK_SIMULATOR_COM_H_

#include <stdint.h>

#include "gmock/gmock.h"

#include "apps/libmc/sim/simulator_com_interface.h"

namespace libmc {
namespace sim {
namespace testing {

// Mock class for SimulatorCom.
class MockSimulatorCom : public SimulatorComInterface {
 public:
  MOCK_METHOD0(Open, bool());

  MOCK_METHOD1(SendMessage, bool(const ProtoMessage &message));
  MOCK_METHOD1(ReceiveMessage, bool(ProtoMessage *message));
};

}  // namespace testing
}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_TESTS_MOCK_SIMULATOR_COM_H_
