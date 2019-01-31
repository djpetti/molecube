#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tachyon/lib/queue_interface.h"
#include "tachyon/test_utils/mock_queue.h"

#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/event_multiplexer.h"
#include "mock_event_listener.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

using ::tachyon::QueueInterface;
using ::tachyon::testing::MockQueue;
using ::testing::SetArgPointee;
using ::testing::_;

// Tests for the EventMultiplexer class.
class EventMultiplexerTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  EventMultiplexerTest()
      : mock_queue_interface_(
            ::std::make_unique<
                MockQueue<EventMultiplexer::EventNotification>>()),
        mock_queue_(
            dynamic_cast<MockQueue<EventMultiplexer::EventNotification> *>(
                mock_queue_interface_.get())),
        multiplexer_(mock_queue_interface_) {}

  // Mock dependencies.
  ::std::unique_ptr<QueueInterface<EventMultiplexer::EventNotification>>
      mock_queue_interface_;
  // This is just a shortcut so we don't have to downcast every time we want to
  // use the mock object.
  MockQueue<EventMultiplexer::EventNotification> *mock_queue_;

  // EventMultiplexer class to use for testing.
  EventMultiplexer multiplexer_;
};

// Tests that Select() works under normal conditions.
TEST_F(EventMultiplexerTest, SelectTest) {
  // Add a listener for an event type.
  MockEventListener listener1;
  multiplexer_.AddListener(&listener1, EventType::SYSTEM);
  // Add a listener for another event type.
  MockEventListener listener2;
  multiplexer_.AddListener(&listener2, EventType::GRAPHICS);

  // Create fake notifications.
  EventMultiplexer::EventNotification notification1 = {EventType::SYSTEM};
  EventMultiplexer::EventNotification notification2 = {EventType::GRAPHICS};

  // Add the notifications to the fake queue.
  EXPECT_CALL(*mock_queue_, DequeueNextBlocking(_))
      .Times(2)
      .WillOnce(SetArgPointee<0>(notification1))
      .WillOnce(SetArgPointee<0>(notification2));

  // Run select.
  EventType type;
  EventListenerInterface *listener_interface = multiplexer_.Select(&type);
  MockEventListener *got_listener =
      dynamic_cast<MockEventListener *>(listener_interface);
  ASSERT_NE(nullptr, got_listener);

  // It should have gotten the correct listener.
  EXPECT_EQ(&listener1, got_listener);
  // It should have selected the correct type.
  EXPECT_EQ(EventType::SYSTEM, type);

  // Run it again.
  listener_interface = multiplexer_.Select(&type);
  got_listener = dynamic_cast<MockEventListener *>(listener_interface);
  ASSERT_NE(nullptr, got_listener);

  EXPECT_EQ(&listener2, got_listener);
  EXPECT_EQ(EventType::GRAPHICS, type);
}

// Tests that Select() handles an unregistered event.
TEST_F(EventMultiplexerTest, SelectUnregisteredTest) {
  // Add a listener for an event type.
  MockEventListener listener;
  multiplexer_.AddListener(&listener, EventType::SYSTEM);

  // Create a fake notifiaction of that type.
  EventMultiplexer::EventNotification notification = {EventType::SYSTEM};
  // Create a fake notification of a different type.
  EventMultiplexer::EventNotification notification_ignored = {
      EventType::GRAPHICS};

  // Add the notifications to the fake queue.
  EXPECT_CALL(*mock_queue_, DequeueNextBlocking(_))
      .Times(2)
      .WillOnce(SetArgPointee<0>(notification_ignored))
      .WillOnce(SetArgPointee<0>(notification));

  // Run select. It should ignore the first one and eventually return the
  // listener for the second one.
  EventType type;
  EventListenerInterface *listener_interface = multiplexer_.Select(&type);
  MockEventListener *got_listener =
      dynamic_cast<MockEventListener *>(listener_interface);
  ASSERT_NE(nullptr, got_listener);

  // It should have gotten the correct listener.
  EXPECT_EQ(&listener, got_listener);
  // It should have selected the correct type.
  EXPECT_EQ(EventType::SYSTEM, type);
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
