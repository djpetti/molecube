#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string.h>

#include <memory>

#include "tachyon/lib/queue_interface.h"
#include "tachyon/test_utils/mock_queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/graphics_event.h"
#include "apps/libmc/core/events/graphics_event_listener.h"
#include "apps/libmc/sim/protobuf/graphics_message.pb.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {
namespace {

using constants::kSimulator;
using sim::GraphicsMessage;
using ::tachyon::QueueInterface;
using ::tachyon::testing::MockQueue;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::_;

// Image data to use for testing.
const uint32_t kNumPixels =
    kSimulator.ScreenWidth * kSimulator.ScreenHeight * 3;
uint8_t kTestImage[kNumPixels];

// Custom matcher for GraphicsEvent structs.
// Args:
//  expected: The expected structure.
MATCHER_P(GraphicsEventEqual, expected, "") {
  return (expected.Common.Type == arg.Common.Type &&
          memcmp(expected.Image, arg.Image, kNumPixels) == 0);
}

// Custom matcher for Protobuf messages.
// Args:
//  expected: The expected message.
MATCHER_P(MessageEqual, expected, "") {
  return (expected.GetTypeName() == arg.GetTypeName() &&
          expected.SerializeAsString() == arg.SerializeAsString());
}

}  // namespace

// Tests for the GraphicsEventListener class.
class GraphicsEventListenerTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  GraphicsEventListenerTest()
      : mock_queue_interface_(::std::make_unique<MockQueue<GraphicsEvent>>()),
        mock_queue_(dynamic_cast<MockQueue<GraphicsEvent> *>(
            mock_queue_interface_.get())),
        listener_(
            GraphicsEventListener::CreateWithQueue(mock_queue_interface_)) {}

  static void SetUpTestCase() {
    // Initialize the testing image.
    for (uint32_t i = 0; i < kNumPixels; ++i) {
      kTestImage[i] = i;
    }
  }

  // Mock dependencies.
  ::std::unique_ptr<QueueInterface<GraphicsEvent>> mock_queue_interface_;
  // This is just a shortcut so we don't have to downcast every time we want to
  // use the mock object.
  MockQueue<GraphicsEvent> *mock_queue_;

  // GraphicsEventListener class to use for testing.
  ::std::unique_ptr<GraphicsEventListener> listener_;
};

// Tests that Listen() works under normal conditions.
TEST_F(GraphicsEventListenerTest, ListenTest) {
  // Create an event to be received.
  GraphicsEvent event;
  event.Common.Type = EventType::GRAPHICS;
  memcpy(event.Image, kTestImage, kNumPixels);

  GraphicsEvent got_event;

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNextBlocking(&got_event))
      .Times(1)
      .WillOnce(SetArgPointee<0>(event));

  listener_->Listen(&(got_event.Common));

  // It should have received the correct event.
  EXPECT_THAT(got_event, GraphicsEventEqual(event));
}

// Tests that Get() works under normal conditions.
TEST_F(GraphicsEventListenerTest, GetTest) {
  // Create an event to be received.
  GraphicsEvent event;
  event.Common.Type = EventType::GRAPHICS;
  memcpy(event.Image, kTestImage, kNumPixels);

  GraphicsEvent got_event;

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNext(&got_event))
      .Times(1)
      .WillOnce(DoAll(SetArgPointee<0>(event), Return(true)));

  EXPECT_TRUE(listener_->Get(&(got_event.Common)));

  // It should have received the correct event.
  EXPECT_THAT(got_event, GraphicsEventEqual(event));
}

// Tests that Get() handles the case when there's no new event.
TEST_F(GraphicsEventListenerTest, GetNoEventTest) {
  GraphicsEvent got_event;

  // It should try to receive the message, but there will be none.
  EXPECT_CALL(*mock_queue_, DequeueNext(&got_event))
      .Times(1)
      .WillOnce(Return(false));

  // This should cause the entire function to fail.
  EXPECT_FALSE(listener_->Get(&(got_event.Common)));
}

// Tests that ListenProtobuf() works under normal conditions.
TEST_F(GraphicsEventListenerTest, ListenProtobufTest) {
  // Create an event to be received.
  GraphicsEvent event;
  event.Common.Type = EventType::GRAPHICS;
  memcpy(event.Image, kTestImage, kNumPixels);

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNextBlocking(_))
      .Times(1)
      .WillOnce(SetArgPointee<0>(event));

  GraphicsMessage message;
  listener_->ListenProtobuf(&message);

  // Create the expected message.
  GraphicsMessage expected;
  expected.set_op_type(GraphicsMessage::PAINT);
  expected.mutable_image()->set_width(kSimulator.ScreenWidth);
  expected.mutable_image()->set_height(kSimulator.ScreenHeight);
  expected.mutable_image()->set_data(kTestImage, kNumPixels);

  // It should have received the correct message.
  EXPECT_THAT(message, MessageEqual(expected));
}

// Tests that GetProtobuf() works under normal conditions.
TEST_F(GraphicsEventListenerTest, GetProtobufTest) {
  // Create an event to be received.
  GraphicsEvent event;
  event.Common.Type = EventType::GRAPHICS;
  memcpy(event.Image, kTestImage, kNumPixels);

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNext(_))
      .Times(1)
      .WillOnce(DoAll(SetArgPointee<0>(event), Return(true)));

  GraphicsMessage message;
  listener_->GetProtobuf(&message);

  // Create the expected message.
  GraphicsMessage expected;
  expected.set_op_type(GraphicsMessage::PAINT);
  expected.mutable_image()->set_width(kSimulator.ScreenWidth);
  expected.mutable_image()->set_height(kSimulator.ScreenHeight);
  expected.mutable_image()->set_data(kTestImage, kNumPixels);

  // It should have received the correct message.
  EXPECT_THAT(message, MessageEqual(expected));
}

// Tests that GetProtobuf() handles the case where getting the event fails.
TEST_F(GraphicsEventListenerTest, GetProtobufNoEventTest) {
  // It should try to receive the message, but there will be none.
  EXPECT_CALL(*mock_queue_, DequeueNext(_))
      .Times(1)
      .WillOnce(Return(false));

  // This should cause the entire function to fail.
  GraphicsMessage message;
  EXPECT_FALSE(listener_->GetProtobuf(&message));
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
