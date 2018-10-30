#include "linux_serial.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace libmc {
namespace sim {

int LinuxSerial::Open(const char *device) {
  return open(device, O_RDWR | O_NOCTTY);
}

bool LinuxSerial::Close(int fd) {
  return close(fd);
}

bool LinuxSerial::TcGetAttr(int fd, termios *attrs) {
  return tcgetattr(fd, attrs) == 0;
}

bool LinuxSerial::TcSetAttr(int fd, int action, const termios *attrs) {
  return tcsetattr(fd, action, attrs) == 0;
}

void LinuxSerial::SetOutSpeed(termios *serial, uint32_t baud) {
  cfsetospeed(serial, static_cast<speed_t>(baud));
}

void LinuxSerial::SetInSpeed(termios *serial, uint32_t baud) {
  cfsetispeed(serial, static_cast<speed_t>(baud));
}

void LinuxSerial::MakeRaw(termios *serial) {
  cfmakeraw(serial);
}

void LinuxSerial::TcFlush(int fd, int options) {
  tcflush(fd, options);
}

int32_t LinuxSerial::Read(int fd, uint8_t *message, uint32_t length) {
  return read(fd, message, length);
}

int32_t LinuxSerial::Write(int fd, const uint8_t *message, uint32_t length) {
  return write(fd, message, length);
}

}  // namespace sim
}  // namespace libmc
