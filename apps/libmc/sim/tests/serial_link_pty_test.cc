#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

#include "apps/libmc/sim/linux_serial.h"
#include "apps/libmc/sim/serial_link.h"

// Needed for grantpt() and unlockpt().
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif  // _XOPEN_SOURCE

namespace libmc {
namespace sim {
namespace testing {

// Tests for the serial link class that use the PTY interface. They don't test
// many corner cases, but instead ensure that the code works in the "real
// world."
class SerialLinkPtyTest : public ::testing::Test {
 protected:
  SerialLinkPtyTest() : link_(&backend_) {}

  virtual void SetUp() {
    // Create a new PTY device.
    ASSERT_TRUE(CreatePty());
    // Open the serial link.
    ASSERT_TRUE(link_.Open(pty_name_, 115200));
  }

  virtual void TearDown() {
    // Destroy the PTY device.
    ClosePty();
  }

  // Receives a message that was sent on the PTY, and checks that it matches
  // some expected value.
  // Args:
  //  expected: The message we expect.
  // Returns:
  //  True if the received message matched the expected message, false
  //  otherwise, or if there was a failure.
  bool ReceiveAndCheckMessage(const char *expected) {
    // Account for the null.
    const uint32_t length = strlen(expected) + 1;

    // Read the message.
    char *actual = new char[length];
    uint32_t actual_read = 0;
    while (actual_read < length) {
      const int32_t read_cycle =
          read(pty_master_, actual + actual_read, length - actual_read);
      if (read_cycle < 0) {
        return false;
      }
      actual_read += read_cycle;
    }

    // Compare the results.
    const bool equal = memcmp(expected, actual, length) == 0;

    delete[] actual;

    return equal;
  }

  // Writes a message to the PTY such that it can be received later.
  // Args:
  //  message: The message to write.
  // Returns:
  //  True upon success, false on failure.
  bool WriteMessage(const char *message) {
    // Account for the null.
    const uint32_t length = strlen(message) + 1;

    uint32_t wrote_bytes = 0;
    while (wrote_bytes < strlen(message) + 1) {
      const int32_t write_cycle =
          write(pty_master_, message + wrote_bytes, length - wrote_bytes);
      if (write_cycle < 0) {
        // Unexpected failure.
        return false;
      }
      wrote_bytes += write_cycle;
    }

    return true;
  }

  // Name of the PTY slave device.
  char *pty_name_;
  // The FD of the PTY master.
  int pty_master_;

  // Backend that we use for serial communication.
  LinuxSerial backend_;
  // The serial link we are testing with.
  SerialLink link_;

 private:
  // Creates and sets the PTY pair to use for this suite.
  // Returns:
  //  True if it successfully created the PTY, false otherwise.
  bool CreatePty() {
    pty_master_ = open("/dev/ptmx", O_RDWR);
    if (pty_master_ < 0) {
      // Failure to open the PTMX for some reason.
      perror("open: ");
      return false;
    }

    if (grantpt(pty_master_)) {
      perror("grantpt: ");
      return false;
    }
    if (unlockpt(pty_master_)) {
      perror("unlockpt: ");
      return false;
    }

    pty_name_ = ptsname(pty_master_);
    return true;
  }

  // Closes the PTY.
  void ClosePty() {
    close(pty_master_);
  }
};

// Tests that we can send a message through the serial link under normal
// conditions.
TEST_F(SerialLinkPtyTest, MessageSendTest) {
  // Try sending some stuff over the link.
  const char *message = "I am a very serious message. Farts.";
  EXPECT_TRUE(link_.SendMessage(reinterpret_cast<const uint8_t *>(message),
                                strlen(message) + 1));

  // Now, receive and compare.
  EXPECT_TRUE(ReceiveAndCheckMessage(message));
}

// Tests that we can receive a message from the serial link under normal
// conditions.
TEST_F(SerialLinkPtyTest, MessageReceiveTest) {
  // Add some stuff to receive.
  const char *message = "Shirley is a lizard person.";
  ASSERT_TRUE(WriteMessage(message));

  // Try receiving it.
  const int32_t length = strlen(message) + 1;
  uint8_t *received = new uint8_t[length];
  EXPECT_TRUE(link_.ReceiveMessage(received, length));

  // Compare the messages.
  EXPECT_EQ(0, memcmp(message, received, length));

  delete[] received;
}

// Same as the above, but tests with a partial message.
TEST_F(SerialLinkPtyTest, PartialMessageReceiveTest) {
  // Add some stuff to receive.
  const char *message = "Shirley is a lizard person.";
  ASSERT_TRUE(WriteMessage(message));

  // Try receiving it.
  const int32_t length = strlen(message) + 1;
  uint8_t *received = new uint8_t[length + 1];
  // Try to receive something longer. It should only receive what it has.
  EXPECT_EQ(length, link_.ReceivePartialMessage(received, length + 1));

  // Compare the messages.
  EXPECT_EQ(0, memcmp(message, received, length));

  delete[] received;
}


}  // namespace testing
}  // namespace sim
}  // namespace libmc
