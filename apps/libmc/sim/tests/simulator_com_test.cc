#include <stdint.h>
#include <string.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/sim/protobuf/test.pb.h"
#include "apps/libmc/sim/simulator_com.h"
#include "mock_cows.h"
#include "mock_serial_link.h"

namespace libmc {
namespace sim {
namespace testing {
namespace {

// Special matcher for verifying that a binary message sent over the wire is
// valid.
// Args:
//  message: The expected message to compare to.
//  length: The expected length of the message, including the initial overhead
//          and zero separator.
MATCHER_P2(BinMessageValid, message, length, "") {
  // Make sure that it has the trailing zeros.
  if (arg[length - 2] != 0) {
    *result_listener << "where the penultimate byte is not zero.";
    return false;
  }
  if (arg[length - 1] != 0) {
    *result_listener << "where the last byte is not zero.";
    return false;
  }

  // Try deserializing the message, and make sure that it matches.
  TestMessage got_message;
  if (!got_message.ParseFromArray(arg + 2, length - 4)) {
    *result_listener << "where parsing the message failed.";
    return false;
  }
  if (got_message.field1() != message.field1()) {
    *result_listener << "where field1 is " << got_message.field1()
                     << " and not " << message.field1();
    return false;
  }
  if (got_message.field2() != message.field2()) {
    *result_listener << "where field2 is " << got_message.field2()
                     << " and not " << message.field2();
    return false;
  }

  return true;
}

// Packet separator value.
const uint8_t kSeparator[] = {0, 0};

}  // namespace

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::StrEq;
using ::testing::StrictMock;

// Tests for the SimulatorCom class.
class SimulatorComTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SimulatorComTest() : com_(&mock_serial_, &mock_cows_) {}

  // Makes it look like synchronizing to the beginning of a packet worked.
  void SetUpPacketSync() {
    // Make it look like we received the separator.
    EXPECT_CALL(mock_serial_, ReceiveMessage(_, 2))
        .Times(1)
        .WillOnce(DoAll(SetArrayArgument<0>(kSeparator, kSeparator + 2),
                        Return(true)));
  }

  // Mock dependencies.
  StrictMock<MockSerialLink> mock_serial_;
  StrictMock<MockCows> mock_cows_;

  // SimulatorCom class to use for testing.
  SimulatorCom com_;
};

// Tests that Open works under normal conditions.
TEST_F(SimulatorComTest, OpenTest) {
  // Set up the mock to look like opening the serial link succeeded.
  EXPECT_CALL(mock_serial_, Open(StrEq(constants::kSimulator.Device),
                                 constants::kSimulator.BaudRate))
      .Times(1)
      .WillOnce(Return(true));
  // Make it look like sending the initial separator also succeeded.
  EXPECT_CALL(mock_serial_, SendMessage(_, 2)).Times(1).WillOnce(Return(true));

  // Perform the call to Open, which should work.
  EXPECT_TRUE(com_.Open());
}

// Tests that Open handles a SerialLink failure.
TEST_F(SimulatorComTest, OpenSerialLinkFailureTest) {
  // Set up the mock to look like opening the serial link failed.
  EXPECT_CALL(mock_serial_, Open(StrEq(constants::kSimulator.Device),
                                 constants::kSimulator.BaudRate))
      .Times(2)
      .WillOnce(Return(false))
      .WillOnce(Return(true));
  // Make it look like sending the initial separator also failed.
  EXPECT_CALL(mock_serial_, SendMessage(_, 2)).Times(1).WillOnce(Return(false));

  // Perform the calls to Open, which should fail twice.
  EXPECT_FALSE(com_.Open());
  EXPECT_FALSE(com_.Open());
}

// Tests that SendMessage works under normal conditions.
TEST_F(SimulatorComTest, SendMessageTest) {
  // Create a test message.
  TestMessage test_message;
  test_message.set_field1(42);
  test_message.set_field2(true);

  const uint32_t message_length = test_message.ByteSizeLong();
  const uint32_t cows_length = message_length + 4;
  const uint32_t padded_length = cows_length + cows_length % 2;

  // Set up expectations for COWS stuffing.
  EXPECT_CALL(mock_cows_, CowsStuff(_, padded_length / 2)).Times(1);
  // It should also send the message over serial.
  EXPECT_CALL(
      mock_serial_,
      SendMessage(BinMessageValid(test_message, cows_length), cows_length))
      .Times(1)
      .WillOnce(Return(true));

  EXPECT_TRUE(com_.SendMessage(test_message));
}

// Tests that SendMessage handles a failure to send the message.
TEST_F(SimulatorComTest, SendMessageFailureTest) {
  // Create a test message.
  TestMessage test_message;
  test_message.set_field1(42);
  test_message.set_field2(true);

  const uint32_t message_length = test_message.ByteSizeLong();
  const uint32_t cows_length = message_length + 4;
  const uint32_t padded_length = cows_length + cows_length % 2;

  // Set up expectations for COWS stuffing.
  EXPECT_CALL(mock_cows_, CowsStuff(_, padded_length / 2)).Times(1);
  // It should fail to send the message over serial.
  EXPECT_CALL(
      mock_serial_,
      SendMessage(BinMessageValid(test_message, cows_length), cows_length))
      .Times(1)
      .WillOnce(Return(false));

  EXPECT_FALSE(com_.SendMessage(test_message));
}

// Tests that we can receive a message under normal conditions.
TEST_F(SimulatorComTest, ReceiveMessageTest) {
  SetUpPacketSync();

  // Create a fake packet to use for this test.
  TestMessage message;
  message.set_field1(42);
  message.set_field2(true);

  const uint32_t message_length = message.ByteSizeLong();
  const uint32_t cows_length = message_length + 4;
  const uint32_t padded_length = cows_length + cows_length % 2;

  uint8_t *unstuffed_packet = new uint8_t[padded_length];
  ASSERT_TRUE(message.SerializeToArray(unstuffed_packet + 2, message_length));

  // Create a fake packet that looks like it could be a stuffed version of the
  // real one. It never actually tries to read the packet without un-stuffing
  // it, so we can get away with this as long as we have the zero separator and
  // no zeros in the middle.
  uint8_t *packet = new uint8_t[padded_length];
  memset(packet, 1, cows_length - 2);
  memset(packet + cows_length - 2, 0, 2);

  // Make ReceivePartialMessage produce this packet.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packet, packet + cows_length),
                      Return(cows_length)));
  // Expect a call to CowsUnstuff. Instead of actually unstuffing the packet,
  // we're going to cheat and substitute our already unstuffed one.
  uint16_t *word_packet = reinterpret_cast<uint16_t *>(unstuffed_packet);
  // We lose one word on the length, because we don't want to parse the
  // separator.
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet,
                                    word_packet + padded_length / 2 - 1));

  TestMessage got_message;
  EXPECT_TRUE(com_.ReceiveMessage(&got_message));

  // Make sure the message we got matches the one we sent.
  EXPECT_EQ(message.field1(), got_message.field1());
  EXPECT_EQ(message.field2(), got_message.field2());

  delete[] packet;
  delete[] unstuffed_packet;
}

// Tests that we can receive a message when it's split into multiple parts.
TEST_F(SimulatorComTest, ReceiveSplitMessageTest) {
  SetUpPacketSync();

  // Create a fake packet to use for this test.
  TestMessage message;
  message.set_field1(42);
  message.set_field2(true);

  const uint32_t message_length = message.ByteSizeLong();
  const uint32_t cows_length = message_length + 4;
  const uint32_t padded_length = cows_length + cows_length % 2;

  uint8_t *unstuffed_packet = new uint8_t[padded_length];
  ASSERT_TRUE(message.SerializeToArray(unstuffed_packet + 2, message_length));

  // Create a fake packet that looks like it could be a stuffed version of the
  // real one. It never actually tries to read the packet without un-stuffing
  // it, so we can get away with this as long as we have the zero separator and
  // no zeros in the middle.
  uint8_t *packet = new uint8_t[padded_length];
  memset(packet, 1, cows_length - 2);
  memset(packet + cows_length - 2, 0, 2);

  // Make ReceivePartialMessage produce this packet, but over the course of two
  // calls.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  EXPECT_CALL(mock_serial_,
              ReceivePartialMessage(_, max_length - cows_length + 1))
      .Times(1)
      .WillOnce(DoAll(
          SetArrayArgument<0>(packet + cows_length - 1, packet + cows_length),
          Return(1)));
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packet, packet + cows_length - 1),
                      Return(cows_length - 1)));
  // Expect a call to CowsUnstuff. Instead of actually unstuffing the packet,
  // we're going to cheat and substitute our already unstuffed one.
  uint16_t *word_packet = reinterpret_cast<uint16_t *>(unstuffed_packet);
  // We lose one word on the length, because we don't want to parse the
  // separator.
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet,
                                    word_packet + padded_length / 2 - 1));

  TestMessage got_message;
  EXPECT_TRUE(com_.ReceiveMessage(&got_message));

  // Make sure the message we got matches the one we sent.
  EXPECT_EQ(message.field1(), got_message.field1());
  EXPECT_EQ(message.field2(), got_message.field2());

  delete[] packet;
  delete[] unstuffed_packet;
}

// Tests that we can receive multiple messages in sequence.
TEST_F(SimulatorComTest, ReceiveMessageMultiMessageTest) {
  SetUpPacketSync();

  // Create a fake packets to use for this test.
  TestMessage message1;
  message1.set_field1(42);
  message1.set_field2(true);

  TestMessage message2;
  message2.set_field1(30);
  message2.set_field2(false);

  const uint32_t message1_length = message1.ByteSizeLong();
  const uint32_t cows_length1 = message1_length + 4;
  const uint32_t padded_length1 = cows_length1 + cows_length1 % 2;
  const uint32_t message2_length = message2.ByteSizeLong();
  const uint32_t cows_length2 = message2_length + 4;
  const uint32_t padded_length2 = cows_length2 + cows_length2 % 2;

  uint8_t *unstuffed_packet1 = new uint8_t[padded_length1];
  uint8_t *unstuffed_packet2 = new uint8_t[padded_length2];
  ASSERT_TRUE(
      message1.SerializeToArray(unstuffed_packet1 + 2, message1_length));
  ASSERT_TRUE(
      message2.SerializeToArray(unstuffed_packet2 + 2, message2_length));

  // Create fake packets that looks like stuffed versions of the
  // real ones. It never actually tries to read the packet without un-stuffing
  // it, so we can get away with this as long as we have the zero separator and
  // no zeros in the middle.
  uint8_t *packets = new uint8_t[padded_length1 + padded_length2];
  memset(packets, 1, padded_length1 + padded_length2);
  memset(packets + cows_length1 - 2, 0, 2);
  memset(packets + cows_length1 + cows_length2 - 2, 0, 2);

  // Make ReceivePartialMessage produce this packet stream, but not broken up
  // neatly.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  const uint32_t stream_length = cows_length1 + cows_length2;
  EXPECT_CALL(mock_serial_,
              ReceivePartialMessage(_, max_length - cows_length2 + 1))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packets + stream_length - 1,
                                          packets + stream_length),
                      Return(1)));
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packets, packets + stream_length - 1),
                      Return(stream_length - 1)));
  // Expect calls to CowsUnstuff. Instead of actually unstuffing the packets,
  // we're going to cheat and substitute our already unstuffed ones.
  uint16_t *word_packet1 = reinterpret_cast<uint16_t *>(unstuffed_packet1);
  uint16_t *word_packet2 = reinterpret_cast<uint16_t *>(unstuffed_packet2);
  // We lose one word on the length, because we don't want to parse the
  // separator.
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length2 / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet2,
                                    word_packet2 + padded_length2 / 2 - 1));
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length1 / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet1,
                                    word_packet1 + padded_length1 / 2 - 1));

  TestMessage got_message1, got_message2;
  EXPECT_TRUE(com_.ReceiveMessage(&got_message1));
  EXPECT_TRUE(com_.ReceiveMessage(&got_message2));

  // Make sure the messages we got match the ones we sent.
  EXPECT_EQ(message1.field1(), got_message1.field1());
  EXPECT_EQ(message1.field2(), got_message1.field2());
  EXPECT_EQ(message2.field1(), got_message2.field1());
  EXPECT_EQ(message2.field2(), got_message2.field2());

  delete[] packets;
  delete[] unstuffed_packet1;
  delete[] unstuffed_packet2;
}

// Tests that we can parse multiple messages when they are received in one
// burst.
TEST_F(SimulatorComTest, ReceiveMessageBlockTest) {
  SetUpPacketSync();

  // Create a fake packets to use for this test.
  TestMessage message1;
  message1.set_field1(42);
  message1.set_field2(true);

  TestMessage message2;
  message2.set_field1(30);
  message2.set_field2(false);

  const uint32_t message1_length = message1.ByteSizeLong();
  const uint32_t cows_length1 = message1_length + 4;
  const uint32_t padded_length1 = cows_length1 + cows_length1 % 2;
  const uint32_t message2_length = message2.ByteSizeLong();
  const uint32_t cows_length2 = message2_length + 4;
  const uint32_t padded_length2 = cows_length2 + cows_length2 % 2;

  uint8_t *unstuffed_packet1 = new uint8_t[padded_length1];
  uint8_t *unstuffed_packet2 = new uint8_t[padded_length2];
  ASSERT_TRUE(
      message1.SerializeToArray(unstuffed_packet1 + 2, message1_length));
  ASSERT_TRUE(
      message2.SerializeToArray(unstuffed_packet2 + 2, message2_length));

  // Create fake packets that looks like stuffed versions of the
  // real ones. It never actually tries to read the packet without un-stuffing
  // it, so we can get away with this as long as we have the zero separator and
  // no zeros in the middle.
  uint8_t *packets = new uint8_t[padded_length1 + padded_length2];
  memset(packets, 1, padded_length1 + padded_length2);
  memset(packets + cows_length1 - 2, 0, 2);
  memset(packets + cows_length1 + cows_length2 - 2, 0, 2);

  // Make ReceivePartialMessage produce this packet stream.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  const uint32_t stream_length = cows_length1 + cows_length2;
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packets, packets + stream_length),
                      Return(stream_length)));
  // Expect calls to CowsUnstuff. Instead of actually unstuffing the packets,
  // we're going to cheat and substitute our already unstuffed ones.
  uint16_t *word_packet1 = reinterpret_cast<uint16_t *>(unstuffed_packet1);
  uint16_t *word_packet2 = reinterpret_cast<uint16_t *>(unstuffed_packet2);
  // We lose one word on the length, because we don't want to parse the
  // separator.
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length2 / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet2,
                                    word_packet2 + padded_length2 / 2 - 1));
  EXPECT_CALL(mock_cows_, CowsUnstuff(_, padded_length1 / 2 - 1))
      .Times(1)
      .WillOnce(SetArrayArgument<0>(word_packet1,
                                    word_packet1 + padded_length1 / 2 - 1));

  TestMessage got_message1, got_message2;
  EXPECT_TRUE(com_.ReceiveMessage(&got_message1));
  EXPECT_TRUE(com_.ReceiveMessage(&got_message2));

  // Make sure the messages we got match the ones we sent.
  EXPECT_EQ(message1.field1(), got_message1.field1());
  EXPECT_EQ(message1.field2(), got_message1.field2());
  EXPECT_EQ(message2.field1(), got_message2.field1());
  EXPECT_EQ(message2.field2(), got_message2.field2());

  delete[] packets;
  delete[] unstuffed_packet1;
  delete[] unstuffed_packet2;
}

// Tests that ReceiveMessage handles a packet that's too long.
TEST_F(SimulatorComTest, ReceiveMessageTooLongTest) {
  SetUpPacketSync();

  // Create a fake packet that deliberately doesn't have a separator.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  uint8_t *packet = new uint8_t[max_length];
  memset(packet, 1, max_length);

  // Make ReceivePartialMessage produce this packet.
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(packet, packet + max_length),
                      Return(max_length)));

  // It should fail to receive this message.
  TestMessage message;
  EXPECT_FALSE(com_.ReceiveMessage(&message));

  delete[] packet;
}

// Tests that ReceiveMessage handles a reading failure.
TEST_F(SimulatorComTest, ReceiveMessageReadFailureTest) {
  SetUpPacketSync();

  // Make ReceivePartialMessage look like it failed.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(Return(-1));

  // It should fail to receive a message.
  TestMessage message;
  EXPECT_FALSE(com_.ReceiveMessage(&message));
}

// Tests that SyncToPacket works under normal conditions.
TEST_F(SimulatorComTest, SyncToPacketTest) {
  // Make it look like it found the packet on its second try.
  const uint8_t non_separator[] = {1, 1};
  EXPECT_CALL(mock_serial_, ReceiveMessage(_, 2))
      .Times(1)
      .WillOnce(DoAll(SetArrayArgument<0>(non_separator, non_separator + 2),
                      Return(true)));
  EXPECT_CALL(mock_serial_, ReceiveMessage(_, 1))
      .Times(3)
      .WillOnce(DoAll(SetArrayArgument<0>(non_separator, non_separator + 1),
                      Return(true)))
      .WillOnce(
          DoAll(SetArrayArgument<0>(kSeparator, kSeparator + 1), Return(true)))
      .WillOnce(DoAll(SetArrayArgument<0>(kSeparator + 1, kSeparator + 2),
                      Return(true)));

  // For the sake of convenience, we're going to make it look like reading the
  // actual packet failed, just so we don't have to mock everything.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  EXPECT_CALL(mock_serial_, ReceivePartialMessage(_, max_length))
      .Times(1)
      .WillOnce(Return(-1));

  // It should fail to receive a message.
  TestMessage message;
  EXPECT_FALSE(com_.ReceiveMessage(&message));
}

// Tests that SyncToPacket handles a read failure.
TEST_F(SimulatorComTest, SyncToPacketReadFailureTest) {
  // Make it look like reading failed.
  EXPECT_CALL(mock_serial_, ReceiveMessage(_, 2))
      .Times(2)
      .WillRepeatedly(Return(false));

  // Receiving a packet should fail immediately.
  TestMessage message;
  EXPECT_FALSE(com_.ReceiveMessage(&message));
  // The state should not be updated, so it should go and try to find the packet
  // start again.
  EXPECT_FALSE(com_.ReceiveMessage(&message));
}

}  // namespace testing
}  // namespace sim
}  // namespace libmc
