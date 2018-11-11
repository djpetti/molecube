#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "apps/libmc/core/events/tests/mock_event_dispatcher.h"
#include "apps/libmc/sim/protobuf/sim_message.pb.h"
#include "apps/libmc/sim/protobuf/system_message.pb.h"
#include "apps/libmc/sim/simulator_process.h"
#include "mock_simulator_com.h"

namespace libmc {
namespace sim {
namespace testing {
namespace {

// Custom action for setting the system submessage in a sim message. It assumes
// that the sim message is the first argument.
// Args:
//  system: The system submessage to set.
ACTION_P(SetSystem, system) {
  SimMessage *sim_message = dynamic_cast<SimMessage *>(arg0);
  sim_message->set_allocated_system(system);
}

}

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

// Tests for the SimulatorProcess class.
class SimulatorProcessTest : public ::testing::Test {
 protected:
  // Inject the mock dependencies.
  SimulatorProcessTest() : process_(&mock_com_, &mock_dispatcher_) {}

  // Mock dependencies.
  StrictMock<MockSimulatorCom> mock_com_;
  StrictMock<core::events::testing::MockEventDispatcher> mock_dispatcher_;

  // SimulatorProcess class to use for testing.
  SimulatorProcess process_;
};

// Tests that run handles a failure to open the com channel.
TEST_F(SimulatorProcessTest, ComOpenFailureTest) {
  // Make it look like opening the serial failed.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(false));

  EXPECT_FALSE(process_.Run());
}

// Tests that Run() works under normal conditions.
TEST_F(SimulatorProcessTest, RunTest) {
  // Create a fake message to receive.
  SystemMessage *sys_message = new SystemMessage;
  // NOTE: It is okay not to free this, because the protobuf system will take
  // ownership of it later.

  // Make it look like opening the serial worked.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(true));
  // Make it look like receiving the message worked the first time. Eventually,
  // we want this to fail so that the Run() call terminates.
  EXPECT_CALL(mock_com_, ReceiveMessage(_))
      .Times(2)
      .WillOnce(DoAll(SetSystem(sys_message),
                      Return(true)))
      .WillOnce(Return(false));
  // Expect the message to be dispatched.
  EXPECT_CALL(mock_dispatcher_, DispatchMessage(_)).Times(1);

  EXPECT_FALSE(process_.Run());
}

// Tests that Run() works when it shouldn't dispatch a system event.
TEST_F(SimulatorProcessTest, RunNoSystemDispatchTest) {
  // Make it look like opening the serial worked.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(true));
  // Make it look like receiving the message worked the first time. Eventually,
  // we want this to fail so that the Run() call terminates.
  EXPECT_CALL(mock_com_, ReceiveMessage(_))
      .Times(2)
      .WillOnce(DoAll(SetSystem(nullptr), Return(true)))
      .WillOnce(Return(false));

  // No system submessage should be set, meaning that nothing should be
  // dispatched.
  EXPECT_FALSE(process_.Run());
}

}  // namespace testing
}  // namespace sim
}  // namespace libmc
