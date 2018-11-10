#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "apps/libmc/core/events/system_event_dispatcher.h"
#include "apps/libmc/core/events/event.h"
#include "mock_system_manager.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

using ::testing::Return;
using ::testing::StrictMock;

// Tests for the SystemEventDispatcher class.
class SystemEventDispatcherTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SystemEventDispatcherTest() : dispatcher_(&mock_manager_) {}

  // Mock dependencies.
  StrictMock<MockSystemManager> mock_manager_;

  // SystemEventDispatcher class to use for testing.
  SystemEventDispatcher dispatcher_;
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

  // Initially, it should not take any action.
  EXPECT_TRUE(dispatcher_.Dispatch(&(event.Common)));

  // It should have set the type correctly.
  EXPECT_EQ(EventType::SYSTEM, event.Common.Type);

  // Now, it should call the manager Shutdown method.
  EXPECT_CALL(mock_manager_, Shutdown()).Times(1).WillOnce(Return(true));

  event.Shutdown = true;
  EXPECT_TRUE(dispatcher_.Dispatch(&(event.Common)));
}

// Tests that Dispatch() fails if Shutdown fails.
TEST_F(SystemEventDispatcherTest, DispatchShutdownFailureTest) {
  SystemEvent event;
  event.Shutdown = true;

  // It should fail if Shutdown fails.
  EXPECT_CALL(mock_manager_, Shutdown()).Times(1).WillOnce(Return(false));
  EXPECT_FALSE(dispatcher_.Dispatch(&(event.Common)));
}

// Tests that the builder version of Dispatch works under normal conditions.
TEST_F(SystemEventDispatcherTest, DispatchBuilderTest) {
  // Build and dispatch an event that does nothing.
  dispatcher_.Dispatch(false);

  EXPECT_CALL(mock_manager_, Shutdown()).Times(1).WillOnce(Return(true));

  // Dispatch one that triggers the shutdown.
  dispatcher_.Dispatch(true);
}

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc
