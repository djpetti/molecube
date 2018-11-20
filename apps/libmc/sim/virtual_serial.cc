#include "virtual_serial.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace libmc {
namespace sim {

int VirtualSerial::Open(const char *device) {
  return open(device, O_RDWR);
}

bool VirtualSerial::Close(int fd) {
  return close(fd);
}

bool VirtualSerial::TcGetAttr(int fd, termios *attrs) {
  return true;
}

bool VirtualSerial::TcSetAttr(int fd, int action, const termios *attrs) {
  return true;
}

void VirtualSerial::SetOutSpeed(termios *serial, uint32_t baud) {}

void VirtualSerial::SetInSpeed(termios *serial, uint32_t baud) {}

void VirtualSerial::MakeRaw(termios *serial) {}

void VirtualSerial::TcFlush(int fd, int options) {}

int32_t VirtualSerial::Read(int fd, uint8_t *message, uint32_t length) {
  return read(fd, message, length);
}

int32_t VirtualSerial::Write(int fd, const uint8_t *message, uint32_t length) {
  return write(fd, message, length);
}

}  // namespace sim
}  // namespace libmc
