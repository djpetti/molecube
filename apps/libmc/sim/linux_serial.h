#ifndef LIBMC_SIM_LINUX_SERIAL_H_
#define LIBMC_SIM_LINUX_SERIAL_H_

#include <termios.h>

#include "linux_serial_interface.h"

namespace libmc {
namespace sim {

// Implementation of LinuxSerialInterface that translates the functions to Linux
// system calls.
class LinuxSerial : public LinuxSerialInterface {
 public:
  virtual int Open(const char *device);
  virtual bool Close(int fd);

  virtual bool TcGetAttr(int fd, termios *attrs);
  virtual bool TcSetAttr(int fd, int action, const termios *attrs);
  virtual void SetOutSpeed(termios *serial, uint32_t baud);
  virtual void SetInSpeed(termios *serial, uint32_t baud);

  virtual void MakeRaw(termios *serial);

  virtual void TcFlush(int fd, int options);

  virtual int32_t Read(int fd, uint8_t *message, uint32_t length);
  virtual int32_t Write(int fd, const uint8_t *message, uint32_t length);
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_LINUX_SERIAL_H_
