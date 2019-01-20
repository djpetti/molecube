#include <string.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "apps/libmc/core/events/tests/mock_event_listener.h"
#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/system_event.h"
#include "apps/libmc/core/system_manager_process.h"
#include "mock_linux_management.h"

namespace libmc {
namespace core {
namespace testing {
namespace {

using events::EventType;
using events::SystemEvent;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

// Custom action for copying a SystemEvent into the structure passed to
// Listen().
// Args:
//  event: The system event to copy.
ACTION_P(SetEvent, event) {
  // We know that the event passed in is going to be of the correct SystemEvent
  // type, so we can just cast it.
  SystemEvent *sys_event_arg = reinterpret_cast<SystemEvent *>(arg0);
  memcpy(sys_event_arg, &event, sizeof(event));
}

}  // namespace

// Tests for the SystemManagerProcess class.
class SystemManagerProcessTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SystemManagerProcessTest() : process_(&mock_listener_, &mock_linux_) {}

  // Mock dependencies.
  StrictMock<events::testing::MockEventListener> mock_listener_;
  StrictMock<MockLinuxManagement> mock_linux_;

  // SystemManagerProcess class to use for testing.
  SystemManagerProcess process_;
};

// Tests that Run() works under normal conditions.
TEST_F(SystemManagerProcessTest, RunTest) {
  // Create a fake event to receive.
  SystemEvent event;
  event.Common.Type = EventType::SYSTEM;
  event.Shutdown = true;

  // Make it look like receiving the event works.
  EXPECT_CALL(mock_listener_, Listen(_))
      .Times(2)
      .WillRepeatedly(SetEvent(event));
  // Expect a call to Sync() before the shutdown.
  EXPECT_CALL(mock_linux_, Sync()).Times(2);
  // Expect it to call the shutdown function. We'll make this fail the second
  // time so that Run() eventually exits.
  EXPECT_CALL(mock_linux_, Halt())
      .Times(2)
      .WillOnce(Return(true))
      .WillOnce(Return(false));

  process_.Run();
}

}  // namespace testing
}  // namespace core
}  // namespace libmc
