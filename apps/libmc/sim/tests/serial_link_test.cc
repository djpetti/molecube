#include <stdint.h>
#include <string.h>
#include <termios.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "apps/libmc/sim/linux_serial_interface.h"
#include "apps/libmc/sim/serial_link.h"
#include "mock_linux_serial.h"

namespace libmc {
namespace sim {
namespace testing {
namespace {

// Constants to use for opening new devices.
const char *kDevice = "/dev/my_happy_device";
const int kFd = 4;
const uint32_t kBaud = 115200;

}  // namespace

using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::_;

// Tests for the serial link class.
class SerialLinkTest : public ::testing::Test {
 protected:
  // Inject the serial layer mock.
  SerialLinkTest() : serial_(&mock_serial_) {}

  virtual void TearDown() {
    // Expect it to try to close the device.
    if (serial_.IsOpen()) {
      EXPECT_CALL(mock_serial_, Close(kFd)).Times(1).WillOnce(Return(true));
    }
  }

  // Helper function for opening the serial port and performing associated
  // tests.
  void OpenSerial() {
    // Set up the mock to look like opening succeeded.
    EXPECT_CALL(mock_serial_, Open(kDevice)).Times(1).WillOnce(Return(kFd));
    // Set up the mock to look like configuration also succeeded.
    EXPECT_CALL(mock_serial_, TcGetAttr(kFd, _)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(mock_serial_, SetInSpeed(_, kBaud)).Times(1);
    EXPECT_CALL(mock_serial_, SetOutSpeed(_, kBaud)).Times(1);
    EXPECT_CALL(mock_serial_, MakeRaw(_)).Times(1);
    EXPECT_CALL(mock_serial_, TcFlush(kFd, _)).Times(1);
    EXPECT_CALL(mock_serial_, TcSetAttr(kFd, _, _))
        .Times(1)
        .WillOnce(Return(true));

    // Perform the call to open, which should work.
    EXPECT_TRUE(serial_.Open(kDevice, kBaud));
    EXPECT_TRUE(serial_.IsOpen());
  }

  // Serial link mocks.
  MockLinuxSerial mock_serial_;

  // Serial link class to use for testing.
  SerialLink serial_;
};

// Tests that Open works under normal conditions.
TEST_F(SerialLinkTest, OpenTest) {
  OpenSerial();
}

// Tests that Open handles a failure to open the device.
TEST_F(SerialLinkTest, OpenDeviceOpenFailureTest) {
  // Set up the mock to look like opening failed.
  EXPECT_CALL(mock_serial_, Open(kDevice)).Times(1).WillOnce(Return(-1));

  // Perform the call to open, which should fail.
  EXPECT_FALSE(serial_.Open(kDevice, kBaud));
  EXPECT_FALSE(serial_.IsOpen());
}

// Tests Open handles a failure to configure the device.
TEST_F(SerialLinkTest, OpenDeviceConfigFailureTest) {
  // Set up the mock to look like opening succeeded.
  EXPECT_CALL(mock_serial_, Open(kDevice)).Times(2).WillRepeatedly(Return(kFd));
  // Set up the mock function to look like getting attributes failed.
  EXPECT_CALL(mock_serial_, TcGetAttr(kFd, _))
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  // Perform the call to open, which should fail.
  EXPECT_FALSE(serial_.Open(kDevice, kBaud));
  EXPECT_FALSE(serial_.IsOpen());

  // The other way this could fail is if setting attributes fails.
  EXPECT_CALL(mock_serial_, TcSetAttr(kFd, _, _))
      .Times(1)
      .WillOnce(Return(false));
  // Everything else should work, though.
  EXPECT_CALL(mock_serial_, SetInSpeed(_, kBaud)).Times(1);
  EXPECT_CALL(mock_serial_, SetOutSpeed(_, kBaud)).Times(1);
  EXPECT_CALL(mock_serial_, MakeRaw(_)).Times(1);
  EXPECT_CALL(mock_serial_, TcFlush(kFd, _)).Times(1);

  // Perform the call to open, which should fail.
  EXPECT_FALSE(serial_.Open(kDevice, kBaud));
  EXPECT_FALSE(serial_.IsOpen());
}

// Tests that SendMessage works under normal conditions.
TEST_F(SerialLinkTest, SendMessageTest) {
  OpenSerial();

  // Message to test with.
  const char *message = "poptart";
  const uint8_t *byte_message = reinterpret_cast<const uint8_t *>(message);
  const uint32_t length = strlen(message) + 1;

  // Make it look like writing succeeded on the first try.
  EXPECT_CALL(mock_serial_, Write(kFd, byte_message, length))
      .Times(1)
      .WillOnce(Return(length));

  EXPECT_TRUE(serial_.SendMessage(byte_message, length));
}

// Tests that SendMessage works when it has to split the message up into parts.
TEST_F(SerialLinkTest, SendMessageSplitTest) {
  OpenSerial();

  // Message to test with.
  const char *message = "My message is bigger than yours.";
  const uint8_t *byte_message = reinterpret_cast<const uint8_t *>(message);
  const uint32_t length = strlen(message) + 1;

  // Make it look like we only wrote part of it on the first try.
  EXPECT_CALL(mock_serial_, Write(kFd, byte_message + length - 1, 1))
      .Times(1)
      .WillOnce(Return(1));
  EXPECT_CALL(mock_serial_, Write(kFd, byte_message, length))
      .Times(1)
      .WillOnce(Return(length - 1));

  EXPECT_TRUE(serial_.SendMessage(byte_message, length));
}

// Tests that SendMessage handles a write failure.
TEST_F(SerialLinkTest, SendMessageWriteFailureTest) {
  OpenSerial();

  // Message to test with.
  const char *message = "poptart";
  const uint8_t *byte_message = reinterpret_cast<const uint8_t *>(message);
  const uint32_t length = strlen(message) + 1;

  // Make it look like writing the message failed.
  EXPECT_CALL(mock_serial_, Write(kFd, byte_message, length))
      .Times(1)
      .WillOnce(Return(-1));

  // Sending the message should fail now.
  EXPECT_FALSE(serial_.SendMessage(byte_message, length));
}

// Tests that ReceiveMessage works under normal conditions.
TEST_F(SerialLinkTest, ReceiveMessageTest) {
  OpenSerial();

  // Message to test with.
  const char *expected_message = "poptart";
  const uint32_t length = strlen(expected_message) + 1;
  uint8_t message[length];
  const uint8_t *byte_ex_message =
      reinterpret_cast<const uint8_t *>(expected_message);

  // Make it look like writing succeeded on the first try.
  EXPECT_CALL(mock_serial_, Read(kFd, message, length))
      .Times(1)
      .WillOnce(
          DoAll(SetArrayArgument<1>(byte_ex_message, byte_ex_message + length),
          Return(length)));

  EXPECT_TRUE(serial_.ReceiveMessage(message, length));

  // Make sure the messages match.
  EXPECT_EQ(0,
            strcmp(expected_message, reinterpret_cast<const char *>(message)));
}

// Tests that ReceiveMessage works when it has to split the message up into parts.
TEST_F(SerialLinkTest, ReceiveMessageSplitTest) {
  OpenSerial();

  // Message to test with.
  const char *expected_message = "My message is bigger than yours.";
  const uint32_t length = strlen(expected_message) + 1;
  uint8_t message[length];
  const uint8_t *byte_ex_message =
      reinterpret_cast<const uint8_t *>(expected_message);

  // Make it look like we only read part of it on the first try.
  EXPECT_CALL(mock_serial_, Read(kFd, message + length - 1, 1))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<1>(byte_ex_message + length - 1,
                                          byte_ex_message + length),
                      Return(1)));
  EXPECT_CALL(mock_serial_, Read(kFd, message, length))
      .Times(1)
      .WillOnce(DoAll(
          SetArrayArgument<1>(byte_ex_message, byte_ex_message + length - 1),
          Return(length - 1)));

  EXPECT_TRUE(serial_.ReceiveMessage(message, length));

  // Make sure the messages match.
  EXPECT_EQ(0,
            strcmp(expected_message, reinterpret_cast<const char *>(message)));
}

// Tests that ReceiveMessage handles a read failure.
TEST_F(SerialLinkTest, SendMessageReadFailureTest) {
  OpenSerial();

  // Message to test with.
  const uint32_t length = 32;
  uint8_t message[length];

  // Make it look like reading the message failed.
  EXPECT_CALL(mock_serial_, Read(kFd, message, length))
      .Times(1)
      .WillOnce(Return(-1));

  // Receiving the message should fail now.
  EXPECT_FALSE(serial_.ReceiveMessage(message, length));
}

int main(int argc, char **argv) {
  // Initialize GoogleMock.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace testing
}  // namespace sim
}  // namespace libmc
