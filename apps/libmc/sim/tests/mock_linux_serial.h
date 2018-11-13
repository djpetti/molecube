#ifndef LIBMC_SIM_TESTS_MOCK_LINUX_SERIAL_H_
#define LIBMC_SIM_TESTS_MOCK_LINUX_SERIAL_H_

#include <stdint.h>
#include <termios.h>

#include "gmock/gmock.h"

#include "apps/libmc/sim/linux_serial_interface.h"

namespace libmc {
namespace sim {
namespace testing {

// Mock class for LinuxSerial.
class MockLinuxSerial : public LinuxSerialInterface {
 public:
  MOCK_METHOD1(Open, int(const char *device));
  MOCK_METHOD1(Close, bool(int fd));

  MOCK_METHOD2(TcGetAttr, bool(int fd, termios *attrs));
  MOCK_METHOD3(TcSetAttr, bool(int fd, int action, const termios *attrs));
  MOCK_METHOD2(SetOutSpeed, void(termios *serial, uint32_t baud));
  MOCK_METHOD2(SetInSpeed, void(termios *serial, uint32_t baud));

  MOCK_METHOD1(MakeRaw, void(termios *serial));

  MOCK_METHOD2(TcFlush, void(int fd, int options));
  MOCK_METHOD3(Read, int32_t(int fd, uint8_t *message, uint32_t length));
  MOCK_METHOD3(Write, int32_t(int fd, const uint8_t *message, uint32_t length));
};

}  // namespace testing
}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_TESTS_MOCK_LINUX_SERIAL_H_
