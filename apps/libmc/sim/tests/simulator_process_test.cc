#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "apps/libmc/core/events/tests/mock_proto_event_dispatcher.h"
#include "apps/libmc/core/events/tests/mock_proto_event_listener.h"
#include "apps/libmc/core/events/tests/mock_event_multiplexer.h"
#include "apps/libmc/sim/protobuf/sim_message.pb.h"
#include "apps/libmc/sim/protobuf/system_message.pb.h"
#include "apps/libmc/sim/protobuf/graphics_message.pb.h"
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

// Custom action for setting the graphics submessage in a sim message. It
// assumes that the sim message is the first argument.
// Args:
//  graphics: The graphics submessage to set.
ACTION_P(SetGraphics, graphics) {
  SimMessage *sim_message = dynamic_cast<SimMessage *>(arg0);
  sim_message->set_allocated_graphics(graphics);
}

// Custom matcher that compares two protobuf message pointers.
// NOTE: This expects the expected message to be a pointer, but the argument
// passed in not to be.
// Args:
//  expected: The message to compare to.
MATCHER_P(ProtobufEqual, expected, "") {
  return (expected->GetTypeName() == arg.GetTypeName() &&
          expected->SerializeAsString() == arg.SerializeAsString());
}

}  // namespace

using core::events::EventType;
using core::events::testing::MockEventMultiplexer;
using core::events::testing::MockProtoEventDispatcher;
using core::events::testing::MockProtoEventListener;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Property;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::WhenDynamicCastTo;

// Tests for the SimulatorProcess class.
class SimulatorProcessTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Every time we call Run(), it's going to internally add the listeners.
    EXPECT_CALL(mock_multiplexer_,
                AddListener(&mock_graphics_listener_, EventType::GRAPHICS))
        .Times(1);

    // Initialize the testing process, injecting the mock dependencies.
    process_ =
        new SimulatorProcess(&mock_com_, &mock_dispatcher_, &mock_multiplexer_,
                             &mock_graphics_listener_);
  }

  // Mock dependencies.
  StrictMock<MockSimulatorCom> mock_com_;
  StrictMock<MockProtoEventDispatcher> mock_dispatcher_;
  StrictMock<MockEventMultiplexer> mock_multiplexer_;
  StrictMock<MockProtoEventListener> mock_graphics_listener_;

  // SimulatorProcess class to use for testing.
  SimulatorProcess *process_;
};

// Tests that run handles a failure to open the com channel.
TEST_F(SimulatorProcessTest, ComOpenFailureTest) {
  // Make it look like opening the serial failed.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(false));

  process_->Run();
}

// Tests that the receiving thread works under normal conditions.
TEST_F(SimulatorProcessTest, ReceivingTest) {
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

  // Make it look like the sending thread failed, so both threads exit.
  EXPECT_CALL(mock_multiplexer_, Select(_))
      .Times(1)
      .WillOnce(Return(&mock_graphics_listener_));
  EXPECT_CALL(mock_graphics_listener_, ListenProtobuf(_)).Times(1);
  EXPECT_CALL(mock_com_, SendMessage(_)).Times(1).WillOnce(Return(false));

  process_->Run();
}

// Tests that the receiving thread works when it shouldn't dispatch a system event.
TEST_F(SimulatorProcessTest, ReceivingNoSystemDispatchTest) {
  // Make it look like opening the serial worked.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(true));
  // Make it look like receiving the message worked the first time. Eventually,
  // we want this to fail so that the Run() call terminates.
  EXPECT_CALL(mock_com_, ReceiveMessage(_))
      .Times(2)
      .WillOnce(DoAll(SetSystem(nullptr), Return(true)))
      .WillOnce(Return(false));

  // Make it look like the sending thread failed, so both threads exit.
  EXPECT_CALL(mock_multiplexer_, Select(_))
      .Times(1)
      .WillOnce(Return(&mock_graphics_listener_));
  EXPECT_CALL(mock_graphics_listener_, ListenProtobuf(_)).Times(1);
  EXPECT_CALL(mock_com_, SendMessage(_)).Times(1).WillOnce(Return(false));

  // No system submessage should be set, meaning that nothing should be
  // dispatched.
  process_->Run();
}

// Tests that the sending thread works under normal conditions.
TEST_F(SimulatorProcessTest, SendingTest) {
  // Create a fake graphics message that will be sent. We have to create two of
  // these, because protobuf auto-frees the old one each time we set a new one.
  GraphicsMessage *graphics_message1 = new GraphicsMessage;
  GraphicsMessage *graphics_message2 = new GraphicsMessage;
  // NOTE: It's okay not to free this, because the protobuf system will take
  // ownership of it later.

  graphics_message1->set_op_type(GraphicsMessage::PAINT);
  graphics_message1->mutable_image()->set_width(100);
  graphics_message1->mutable_image()->set_height(200);

  graphics_message2->set_op_type(GraphicsMessage::PAINT);
  graphics_message2->mutable_image()->set_width(50);
  graphics_message2->mutable_image()->set_height(300);

  // Make it look like opening the serial worked.
  EXPECT_CALL(mock_com_, Open()).Times(1).WillOnce(Return(true));

  // Make it look like we got a graphics event.
  EXPECT_CALL(mock_multiplexer_, Select(_))
      .Times(2)
      .WillRepeatedly(Return(&mock_graphics_listener_));
  EXPECT_CALL(mock_graphics_listener_, ListenProtobuf(_))
      .Times(2)
      .WillOnce(SetGraphics(graphics_message1))
      .WillOnce(SetGraphics(graphics_message2));
  // Make it look like sending the message initially worked, but failed the
  // second time, so that Run() eventually terminates.
  EXPECT_CALL(mock_com_,
              SendMessage(WhenDynamicCastTo<const SimMessage *>(Property(
                  &SimMessage::graphics, ProtobufEqual(graphics_message1)))))
      .Times(1)
      .WillOnce(Return(true));
  EXPECT_CALL(mock_com_,
              SendMessage(WhenDynamicCastTo<const SimMessage *>(Property(
                  &SimMessage::graphics, ProtobufEqual(graphics_message2)))))
      .Times(1)
      .WillOnce(Return(false));

  // Make it look like the receiving thread failed, so it exits immediately.
  EXPECT_CALL(mock_com_, ReceiveMessage(_)).Times(1).WillOnce(Return(false));

  process_->Run();
}

}  // namespace testing
}  // namespace sim
}  // namespace libmc
