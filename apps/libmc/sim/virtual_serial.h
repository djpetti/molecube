#ifndef LIBMC_SIM_VIRTUAL_SERIAL_H_
#define LIBMC_SIM_VIRTUAL_SERIAL_H_

#include <termios.h>

#include "linux_serial_interface.h"

namespace libmc {
namespace sim {

// Implementation of LinuxSerialInterface that talks to a QEMU virtual serial
// device. That term is slightly confusing, because in QEMU, serial devices are
// not actually ttys and end up behaving more like pipes. Consequently, this
// class implements the full functionality of the interface, but many of the
// calls applying only to ttys do nothing at all.
class VirtualSerial : public LinuxSerialInterface {
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

#endif  // LIBMC_SIM_VIRTUAL_SERIAL_H_
