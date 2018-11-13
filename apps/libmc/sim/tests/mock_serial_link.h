#ifndef LIBMC_SIM_TESTS_MOCK_SERIAL_LINK_H_
#define LIBMC_SIM_TESTS_MOCK_SERIAL_LINK_H_

#include <stdint.h>

#include "gmock/gmock.h"

#include "apps/libmc/sim/serial_link_interface.h"

namespace libmc {
namespace sim {
namespace testing {

// Mock class for SerialLink.
class MockSerialLink : public SerialLinkInterface {
 public:
  MOCK_METHOD2(Open, bool(const char *device, uint32_t baud));
  MOCK_METHOD0(IsOpen, bool());

  MOCK_METHOD2(SendMessage, bool(const uint8_t *message, uint32_t length));
  MOCK_METHOD2(ReceiveMessage, bool(uint8_t *message, uint32_t length));
  MOCK_METHOD2(ReceivePartialMessage,
               int32_t(uint8_t *message, uint32_t length));
};

}  // namespace testing
}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_TESTS_MOCK_SERIAL_LINK_H_
