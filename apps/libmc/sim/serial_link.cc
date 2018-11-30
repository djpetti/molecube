#include "serial_link.h"

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>

#include "linux_serial.h"

namespace libmc {
namespace sim {

SerialLink::SerialLink(LinuxSerialInterface *os_serial) : os_(os_serial) {}

SerialLink::~SerialLink() {
  // Close the serial device.
  if (IsOpen()) {
    (void)os_->Close(serial_);
  }
}

bool SerialLink::Open(const char *device, uint32_t baud) {
  // Open the serial link.
  serial_ = os_->Open(device);
  if (serial_ < 0) {
    // Failure to open device.
    return false;
  }

  // Configure the link.
  if (!ConfigureSerial(baud)) {
    serial_ = -1;
    return false;
  }

  return true;
}

bool SerialLink::IsOpen() {
  return serial_ != -1;
}

bool SerialLink::ConfigureSerial(uint32_t baud) {
  struct termios tty;
  memset(&tty, 0, sizeof(tty));

  if (!os_->TcGetAttr(serial_, &tty)) {
    // Failure to get initial attributes.
    return false;
  }

  // Set the baud rate.
  os_->SetInSpeed(&tty, baud);
  os_->SetOutSpeed(&tty, baud);

  // Make 8n1.
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  // No flow control.
  tty.c_cflag &= ~CRTSCTS;

  // Use raw mode.
  os_->MakeRaw(&tty);

  // Flush port and apply attributes.
  os_->TcFlush(serial_, TCIFLUSH);
  if (!os_->TcSetAttr(serial_, TCSANOW, &tty)) {
    // Failure to set attributes.
    return false;
  }

  return true;
}

bool SerialLink::SendMessage(const uint8_t *message, uint32_t length) {
  uint32_t remaining = length;
  while (remaining > 0) {
    const int32_t write_ret = os_->Write(serial_, message, remaining);
    if (write_ret < 0) {
      // Write failure.
      return false;
    }

    remaining -= write_ret;
    message += write_ret;
  }

  return true;
}

bool SerialLink::ReceiveMessage(uint8_t *message, uint32_t length) {
  uint32_t remaining = length;
  while (remaining > 0) {
    const int32_t read_ret = os_->Read(serial_, message, remaining);
    if (read_ret < 0) {
      // Read failure.
      return false;
    }

    remaining -= read_ret;
    message += read_ret;
  }

  return true;
}

int32_t SerialLink::ReceivePartialMessage(uint8_t *message,
                                          uint32_t max_length) {
  // For receiving, we're fine with reading a partial message.
  return os_->Read(serial_, message, max_length);
}

}  // namespace sim
}  // namespace libmc
