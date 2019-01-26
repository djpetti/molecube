#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

#include "tachyon/lib/queue_interface.h"
#include "tachyon/test_utils/mock_queue.h"

#include "apps/libmc/core/events/system_event_listener.h"
#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/system_event.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

using ::tachyon::testing::MockQueue;
using ::tachyon::QueueInterface;

using ::testing::DoAll;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::SetArgPointee;

// Custom matcher for SystemEvent structs.
// Args:
//  expected: The expected structure.
MATCHER_P(SystemEventEqual, expected, "") {
  return (expected.Common.Type == arg.Common.Type &&
          expected.Shutdown == arg.Shutdown);
}

// Tests for the SystemEventListener class.
class SystemEventListenerTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SystemEventListenerTest()
      : mock_queue_interface_(::std::make_unique<MockQueue<SystemEvent>>()),
        mock_queue_(dynamic_cast<MockQueue<SystemEvent> *>(
            mock_queue_interface_.get())),
        listener_(
            SystemEventListener::CreateWithQueue(mock_queue_interface_)) {}

  // Mock dependencies.
  ::std::unique_ptr<QueueInterface<SystemEvent>> mock_queue_interface_;
  // This is just a shortcut so we don't have to downcast every time we want to
  // use the mock object.
  MockQueue<SystemEvent> *mock_queue_;

  // SystemEventListener class to use for testing.
  ::std::unique_ptr<SystemEventListener> listener_;
};

// Tests that using the singleton interface works.
TEST_F(SystemEventListenerTest, SingletonTest) {
  // Try getting the instance.
  const SystemEventListener &handle1 = SystemEventListener::GetInstance();
  // Get it again.
  const SystemEventListener &handle2 = SystemEventListener::GetInstance();

  // They should be the same.
  EXPECT_EQ(&handle1, &handle2);
}

// Tests that Listen() works under normal conditions.
TEST_F(SystemEventListenerTest, ListenTest) {
  // Create an event to be received.
  SystemEvent event;
  event.Common.Type = EventType::SYSTEM;
  event.Shutdown = false;

  SystemEvent next_event = event;
  next_event.Shutdown = true;

  SystemEvent got_event;

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNextBlocking(&got_event))
      .Times(2)
      .WillOnce(SetArgPointee<0>(event))
      .WillOnce(SetArgPointee<0>(next_event));

  listener_->Listen(&(got_event.Common));

  // It should have received the correct event.
  EXPECT_EQ(EventType::SYSTEM, got_event.Common.Type);
  EXPECT_FALSE(got_event.Shutdown);

  // This should work even when we tell it to shutdown.
  listener_->Listen(&(got_event.Common));

  EXPECT_EQ(EventType::SYSTEM, got_event.Common.Type);
  EXPECT_TRUE(got_event.Shutdown);
}

// Tests that Get() works under normal conditions.
TEST_F(SystemEventListenerTest, GetTest) {
  // Create an event to be received.
  SystemEvent event;
  event.Common.Type = EventType::SYSTEM;
  event.Shutdown = false;

  SystemEvent next_event = event;
  next_event.Shutdown = true;

  SystemEvent got_event;

  // It should receive the message.
  EXPECT_CALL(*mock_queue_, DequeueNext(&got_event))
      .Times(2)
      .WillOnce(DoAll(SetArgPointee<0>(event), Return(true)))
      .WillOnce(DoAll(SetArgPointee<0>(next_event), Return(true)));

  EXPECT_TRUE(listener_->Get(&(got_event.Common)));

  // It should have received the correct event.
  EXPECT_EQ(EventType::SYSTEM, got_event.Common.Type);
  EXPECT_FALSE(got_event.Shutdown);

  // This should work even when we tell it to shutdown.
  EXPECT_TRUE(listener_->Get(&(got_event.Common)));

  EXPECT_EQ(EventType::SYSTEM, got_event.Common.Type);
  EXPECT_TRUE(got_event.Shutdown);
}

// Tests that Get() handles the case when there's no new event.
TEST_F(SystemEventListenerTest, GetNoEventTest) {
  SystemEvent got_event;

  // It should try to receive the message, but there will be none.
  EXPECT_CALL(*mock_queue_, DequeueNext(&got_event))
      .Times(1)
      .WillOnce(Return(false));

  // This should cause the entire function to fail.
  EXPECT_FALSE(listener_->Get(&(got_event.Common)));
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
