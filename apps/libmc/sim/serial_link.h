#ifndef LIBMC_SIM_SERIAL_LINK_H_
#define LIBMC_SIM_SERIAL_LINK_H_

#include <stdint.h>

#include "linux_serial_interface.h"

namespace libmc {
namespace sim {

// Manages low-level communication on the serial link with the simulation host.
class SerialLink {
 public:
  // Default constructor should be used normally.
  SerialLink();
  // Alternate constructor that allows us to do dependency injection during
  // testing.
  // Args:
  //  os_serial: The OS serial interface layer to use.
  SerialLink(LinuxSerialInterface *os_serial);
  ~SerialLink();

  // Initializes the serial link.
  // Args:
  //  device: The serial device to communicate on.
  //  baud: The baudrate to use for communication.
  // Returns:
  //  True if opening the connection succeeded, false otherwise.
  bool Open(const char *device, uint32_t baud);
  // Returns true if the link is open, false otherwise.
  bool IsOpen();

  // Send a message on the serial device.
  // Args:
  //  message: The message to send.
  //  length: The length of the message.
  // Returns:
  //  True if sending the message was successful, false otherwise.
  bool SendMessage(const uint8_t *message, uint32_t length);
  // Receive a message from the serial device.
  // Args:
  //  message: Where to write the received message.
  //  length: The length of the message to receive.
  // Returns:
  //  True if the message was successfully received, false otherwise
  bool ReceiveMessage(uint8_t *message, uint32_t length);

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
