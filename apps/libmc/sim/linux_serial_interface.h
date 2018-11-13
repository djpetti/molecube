#ifndef LIBMC_SIM_LINUX_SERIAL_INTERFACE_H_
#define LIBMC_SIM_LINUX_SERIAL_INTERFACE_H_

#include <stdint.h>
#include <termios.h>

namespace libmc {
namespace sim {

// Defines an interface for anything that depends on the linux serial system.
// This is meant to allow us to easily mock functions.
class LinuxSerialInterface {
 public:
  virtual ~LinuxSerialInterface() = default;

  // Opens a new serial device.
  // Args:
  //  device: The name of the device to open.
  // Returns:
  //  The serial device FD, or -1 on failure.
  virtual int Open(const char *device) = 0;
  // Closes a serial device.
  // Args:
  //  fd: The FD of the device to close.
  // Returns:
  //  True if it closed sucessfully, false otherwise.
  virtual bool Close(int fd) = 0;

  // Gets terminal attributes.
  // Args:
  //  fd: The FD to get attributes of.
  //  attrs: Where to write the attributes.
  // Returns:
  //  True on success, false otherwise.
  virtual bool TcGetAttr(int fd, termios *attrs) = 0;
  // Sets terminal attributes.
  // Args:
  //  fd: The FD to set attributes of.
  //  action: Constant defining optional actions to take.
  //  attrs: The attributes to set.
  // Returns:
  //  True on success, false otherwise.
  virtual bool TcSetAttr(int fd, int action, const termios *attrs) = 0;
  // Sets the output baud rate.
  // Args:
  //  serial: The device to set the baud rate on.
  //  baud: The baud rate to set.
  virtual void SetOutSpeed(termios *serial, uint32_t baud) = 0;
  // Sets the input baud rate.
  // Args:
  //  serial: The device to set the baud rate on.
  //  baud: The baud rate to set.
  virtual void SetInSpeed(termios *serial, uint32_t baud) = 0;

  // Enables "raw" mode for a serial device.
  // Args:
  //  serial: The device to enable raw mode on.
  virtual void MakeRaw(termios *serial) = 0;

  // Flushes the serial port.
  // Args:
  //  fd: The FD of the port to flush.
  //  options: Constant defining options.
  virtual void TcFlush(int fd, int options) = 0;

  // Reads from the serial port.
  // Args:
  //  fd: The FD of the port to read from.
  //  message: Buffer to store the message in.
  //  length: Maximum number of bytes to read.
  // Returns:
  //  The number of bytes that were actually read, or -1 upon error.
  virtual int32_t Read(int fd, uint8_t *message, uint32_t length) = 0;
  // Writes to the serial port.
  // Args:
  //  fd: The FD of the port to write to.
  //  message: The message to write.
  //  length: The maximum number of bytes to write.
  // Returns:
  //  The number of bytes that were actually written, or -1 upon error.
  virtual int32_t Write(int fd, const uint8_t *message, uint32_t length) = 0;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_LINUX_SERIAL_INTERFACE_H_
