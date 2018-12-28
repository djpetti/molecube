#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

#include "tachyon/lib/queue_interface.h"
#include "tachyon/test_utils/mock_queue.h"

#include "apps/libmc/core/events/system_event_dispatcher.h"
#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/system_event.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

using ::tachyon::testing::MockQueue;
using ::tachyon::QueueInterface;
using ::testing::Return;
using ::testing::StrictMock;

// Custom matcher for SystemEvent structs.
// Args:
//  expected: The expected structure.
MATCHER_P(SystemEventEqual, expected, "") {
  return (expected.Common.Type == arg.Common.Type &&
          expected.Shutdown == arg.Shutdown);
}

// Tests for the SystemEventDispatcher class.
class SystemEventDispatcherTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SystemEventDispatcherTest()
      : mock_queue_interface_(::std::make_unique<MockQueue<SystemEvent>>()),
        mock_queue_(dynamic_cast<MockQueue<SystemEvent> *>(
            mock_queue_interface_.get())),
        dispatcher_(
            SystemEventDispatcher::CreateWithQueue(mock_queue_interface_)) {}

  // Mock dependencies.
  ::std::unique_ptr<QueueInterface<SystemEvent>> mock_queue_interface_;
  // This is just a shortcut so we don't have to downcast every time we want to
  // use the mock object.
  MockQueue<SystemEvent> *mock_queue_;

  // SystemEventDispatcher class to use for testing.
  SystemEventDispatcher &dispatcher_;
};

// Tests that using the singleton interface works.
TEST_F(SystemEventDispatcherTest, SingletonTest) {
  // Try getting the instance.
  const SystemEventDispatcher &handle1 = SystemEventDispatcher::GetInstance();
  // Get it again.
  const SystemEventDispatcher &handle2 = SystemEventDispatcher::GetInstance();

  // They should be the same.
  EXPECT_EQ(&handle1, &handle2);
}

// Tests that Dispatch() works under normal conditions.
TEST_F(SystemEventDispatcherTest, DispatchTest) {
  // Create an event to dispatch.
  SystemEvent event;
  event.Shutdown = false;

  SystemEvent expected = event;
  expected.Common.Type = EventType::SYSTEM;

  // It should send the queue message.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(SystemEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));
  expected.Shutdown = true;
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(SystemEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));

  EXPECT_TRUE(dispatcher_.Dispatch(&(event.Common)));

  // It should have set the fields correctly.
  EXPECT_EQ(EventType::SYSTEM, event.Common.Type);
  EXPECT_FALSE(event.Shutdown);

  // This should work even if we tell it to shut down.
  event.Shutdown = true;
  EXPECT_TRUE(dispatcher_.Dispatch(&(event.Common)));

  EXPECT_EQ(EventType::SYSTEM, event.Common.Type);
  EXPECT_TRUE(event.Shutdown);
}

// Tests that Dispatch() fails if sending the message fails.
TEST_F(SystemEventDispatcherTest, DispatchMessageSendFailureTest) {
  SystemEvent event;
  event.Shutdown = true;

  SystemEvent expected = event;
  expected.Common.Type = EventType::SYSTEM;

  // It should fail if EnqueueBlocking fails.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(SystemEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(false));
  EXPECT_FALSE(dispatcher_.Dispatch(&(event.Common)));
}

// Tests that the builder version of Dispatch works under normal conditions.
TEST_F(SystemEventDispatcherTest, DispatchBuilderTest) {
  // Event that we expect to see dispatched.
  SystemEvent expected;
  expected.Common.Type = EventType::SYSTEM;
  expected.Shutdown = false;

  // Build and dispatch an event that does nothing.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(SystemEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));
  EXPECT_TRUE(dispatcher_.Dispatch(false));

  expected.Shutdown = true;

  // Dispatch one that triggers the shutdown.
  EXPECT_CALL(*mock_queue_, EnqueueBlocking(SystemEventEqual(expected)))
      .Times(1)
      .WillOnce(Return(true));
  EXPECT_TRUE(dispatcher_.Dispatch(true));
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
