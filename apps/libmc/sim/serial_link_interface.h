#ifndef LIBMC_SIM_SERIAL_LINK_INTERFACE_H_
#define LIBMC_SIM_SERIAL_LINK_INTERFACE_H_

#include <stdint.h>

namespace libmc {
namespace sim {

// Defines abstract interface for the SerialLink class.
class SerialLinkInterface {
 public:
  virtual ~SerialLinkInterface() = default;

  // Initializes the serial link.
  // Args:
  //  device: The serial device to communicate on.
  //  baud: The baudrate to use for communication.
  // Returns:
  //  True if opening the connection succeeded, false otherwise.
  virtual bool Open(const char *device, uint32_t baud) = 0;
  // Returns true if the link is open, false otherwise.
  virtual bool IsOpen() = 0;

  // Send a message on the serial device.
  // Args:
  //  message: The message to send.
  //  length: The length of the message.
  // Returns:
  //  True if sending the message was successful, false otherwise.
  virtual bool SendMessage(const uint8_t *message, uint32_t length) = 0;
  // Receive a message from the serial device.
  // Args:
  //  message: Where to write the received message.
  //  length: The length of the message to receive.
  // Returns:
  //  True if it successfully received the message, false otherwise.
  virtual bool ReceiveMessage(uint8_t *message, uint32_t length) = 0;
  // Receive a message from the serial device. It might not read the entire
  // length.
  // Args:
  //  message: Where to write the received message.
  //  length: The length of the message to receive.
  // Returns:
  //  The number of bytes total that it received.
  virtual int32_t ReceivePartialMessage(uint8_t *message, uint32_t length) = 0;
};

}  // namespace libmc
}  // namespace sim

#endif  // LIBMC_SIM_SERIAL_LINK_INTERFACE_H_
