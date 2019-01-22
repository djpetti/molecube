#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string.h>

#include <memory>

#include "tachyon/lib/queue_interface.h"
#include "tachyon/test_utils/mock_queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/graphics_event.h"
#include "apps/libmc/core/events/graphics_event_dispatcher.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {
namespace {

using constants::kSimulator;
using ::tachyon::QueueInterface;
using ::tachyon::testing::MockQueue;
using ::testing::Return;
using ::testing::StrictMock;

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

}  // namespace

// Tests for the GraphicsEventDispatcher class.
class GraphicsEventDispatcherTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  GraphicsEventDispatcherTest()
      : mock_queue_interface_(::std::make_unique<MockQueue<GraphicsEvent>>()),
        mock_queue_(dynamic_cast<MockQueue<GraphicsEvent> *>(
            mock_queue_interface_.get())),
        dispatcher_(
            GraphicsEventDispatcher::CreateWithQueue(mock_queue_interface_)) {}

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

  // GraphicsEventDispatcher class to use for testing.
  GraphicsEventDispatcher &dispatcher_;
};

// Tests that using the singleton interface works.
TEST_F(GraphicsEventDispatcherTest, SingletonTest) {
  // Try getting the instance.
  const GraphicsEventDispatcher &handle1 =
      GraphicsEventDispatcher::GetInstance();
  // Get it again.
  const GraphicsEventDispatcher &handle2 =
      GraphicsEventDispatcher::GetInstance();

  // They should be the same.
  EXPECT_EQ(&handle1, &handle2);
}

// Tests that Dispatch() works under normal conditions.
TEST_F(GraphicsEventDispatcherTest, DispatchTest) {
  // Create an event to dispatch.
  GraphicsEvent event;
  memcpy(event.Image, kTestImage, kNumPixels);

  GraphicsEvent expected = event;
  expected.Common.Type = EventType::GRAPHICS;

  // It should send the queue message.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(GraphicsEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));

  EXPECT_TRUE(dispatcher_.Dispatch(&(event.Common)));

  // It should have set the fields correctly.
  EXPECT_THAT(event, GraphicsEventEqual(expected));
}

// Tests that Dispatch() fails if sending the message fails.
TEST_F(GraphicsEventDispatcherTest, DispatchMessageSendFailureTest) {
  GraphicsEvent event;
  memcpy(event.Image, kTestImage, kNumPixels);

  GraphicsEvent expected = event;
  expected.Common.Type = EventType::GRAPHICS;

  // It should fail if EnqueueBlocking fails.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(GraphicsEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(false));
  EXPECT_FALSE(dispatcher_.Dispatch(&(event.Common)));
}

// Tests that the builder version of Dispatch() works under normal conditions.
TEST_F(GraphicsEventDispatcherTest, DispatchBuilderTest) {
  // Event that we expect to see dispatched.
  GraphicsEvent expected;
  expected.Common.Type = EventType::GRAPHICS;
  memcpy(expected.Image, kTestImage, kNumPixels);

  // Build and dispatch an event.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(GraphicsEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));
  EXPECT_TRUE(dispatcher_.Dispatch(kTestImage));
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
