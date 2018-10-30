#ifndef LIBMC_SIM_SERIAL_LINK_H_
#define LIBMC_SIM_SERIAL_LINK_H_

#include <stdint.h>

#include "linux_serial_interface.h"
#include "serial_link_interface.h"

namespace libmc {
namespace sim {

// Manages low-level communication on the serial link with the simulation host.
class SerialLink : public SerialLinkInterface {
 public:
  // Default constructor should be used normally.
  SerialLink();
  // Alternate constructor that allows us to do dependency injection during
  // testing.
  // Args:
  //  os_serial: The OS serial interface layer to use.
  SerialLink(LinuxSerialInterface *os_serial);
  virtual ~SerialLink();

  virtual bool Open(const char *device, uint32_t baud);
  virtual bool IsOpen();

  virtual bool SendMessage(const uint8_t *message, uint32_t length);
  virtual bool ReceiveMessage(uint8_t *message, uint32_t length);
  virtual int32_t ReceivePartialMessage(uint8_t *message, uint32_t length);

 private:
  // Configures the internal serial link.
  // Args:
  //  The baud rate to use.
  bool ConfigureSerial(uint32_t baud);

  // The FD of the serial device.
  int serial_ = -1;
  // Handle for the OS serial interface.
  LinuxSerialInterface *os_;
  // Keeps track of whether we own the handle.
  bool own_os_ = true;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_SERIAL_LINK_H_
