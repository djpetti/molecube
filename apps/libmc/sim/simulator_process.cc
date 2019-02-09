#include "simulator_process.h"

#include <thread>

#include "glog/logging.h"

#include "apps/libmc/core/events/event_listener_interface.h"
#include "apps/libmc/core/events/event_multiplexer.h"
#include "apps/libmc/core/events/graphics_event_listener.h"
#include "apps/libmc/core/events/system_event_dispatcher.h"
#include "apps/libmc/sim/protobuf/graphics_message.pb.h"
#include "simulator_com.h"

namespace libmc {
namespace sim {

using core::events::EventListenerInterface;
using core::events::EventMultiplexer;
using core::events::EventMultiplexerInterface;
using core::events::EventType;
using core::events::GraphicsEventListener;
using core::events::ProtoEventDispatcher;
using core::events::ProtoEventListenerInterface;
using core::events::SystemEventDispatcher;
using ::google::protobuf::MessageLite;

SimulatorProcess::SimulatorProcess()
    : SimulatorProcess(new SimulatorCom, &SystemEventDispatcher::GetInstance(),
                       new EventMultiplexer) {
  own_com_ = true;
  own_multiplexer_ = true;
  own_listeners_ = true;
}

SimulatorProcess::SimulatorProcess(
    SimulatorComInterface *com, ProtoEventDispatcher *dispatcher,
    EventMultiplexerInterface *multiplexer,
    ProtoEventListenerInterface *graphics_listener)
    : SimulatorProcess(com, dispatcher, multiplexer) {
  // Add the listeners to the multiplexer.
  multiplexer_->AddListener(graphics_listener, EventType::GRAPHICS);
}

SimulatorProcess::SimulatorProcess(
    SimulatorComInterface *com, core::events::ProtoEventDispatcher *dispatcher,
    core::events::EventMultiplexerInterface *multiplexer)
    : com_(com), dispatcher_(dispatcher), multiplexer_(multiplexer) {}

SimulatorProcess::~SimulatorProcess() {
  // If we own members, we have to delete them.
  if (own_com_) {
    delete com_;
  }
  if (own_multiplexer_) {
    delete multiplexer_;
  }
}

void SimulatorProcess::Run() {
  // Perform set-up.
  if (!SetUp()) {
    return;
  }

  // Start the threads.
  LOG(INFO) << "Starting threads.";
  ::std::thread send_thread(&SimulatorProcess::SendingThread, this);
  ::std::thread recv_thread(&SimulatorProcess::ReceivingThread, this);

  // Wait for the threads to exit.
  send_thread.join();
  recv_thread.join();
}

bool SimulatorProcess::SetUp() {
  // First, open the com channel.
  if (!com_->Open()) {
    return false;
  }

  if (own_listeners_) {
    // Get listeners and add them to the multiplexer.
    auto &graphics_listener = GraphicsEventListener::GetInstance();
    multiplexer_->AddListener(&graphics_listener, EventType::GRAPHICS);
  }

  return true;
}

bool SimulatorProcess::HandleSerialMessage() {
  VLOG(1) << "Waiting to receive message.";

  // Wait for the next message from the serial.
  SimMessage message;
  if (!com_->ReceiveMessage(&message)) {
    // Failed to receive the message.
    return false;
  }

  // Check if any of the submessages were set and dispatch them if they were.
  if (message.has_system()) {
    LOG(INFO) << "Dispatching system message.";
    dispatcher_->DispatchMessage(&message.system());
  }

  return true;
}

bool SimulatorProcess::HandleEvent() {
  VLOG(1) << "Waiting to receive event.";

  // Wait for the next event.
  EventType type;
  EventListenerInterface *listener = multiplexer_->Select(&type);
  // If we're using it with the simulator process, the listener should know how
  // to emit Protobuf messages.
  ProtoEventListenerInterface *proto_listener =
      dynamic_cast<ProtoEventListenerInterface *>(listener);
  DLOG_IF(FATAL, !proto_listener)
      << "All listeners must support Protobuf output.";

  // Populate the correct sub-message.
  proto_listener->ListenProtobuf(&sim_message_);

  // Send the message over the serial.
  VLOG(1) << "Sending message.";
  if (!com_->SendMessage(&sim_message_)) {
    return false;
  }

  return true;
}

void SimulatorProcess::ReceivingThread() {
  LOG(INFO) << "Starting receiving thread.";

  while (HandleSerialMessage());

  LOG(ERROR) << "Exiting receiving thread.";
}

void SimulatorProcess::SendingThread() {
  LOG(INFO) << "Starting sending thread.";

  while (HandleEvent());

  LOG(ERROR) << "Exiting sending thread.";
}

}  // namespace sim
}  // namespace libmc
