#include "simulator_process.h"

#include "glog/logging.h"

#include "google/protobuf/message.h"

#include "apps/libmc/core/events/system_event_dispatcher.h"
#include "apps/libmc/sim/protobuf/sim_message.pb.h"
#include "simulator_com.h"

namespace libmc {
namespace sim {

using core::events::ProtoEventDispatcherInterface;
using core::events::SystemEventDispatcher;
using ::google::protobuf::MessageLite;

SimulatorProcess::SimulatorProcess()
    : SimulatorProcess(new SimulatorCom,
                       &SystemEventDispatcher::GetInstance()) {
  own_com_ = true;
}

SimulatorProcess::SimulatorProcess(SimulatorComInterface *com,
                                   ProtoEventDispatcherInterface *dispatcher)
    : com_(com), dispatcher_(dispatcher) {}

SimulatorProcess::~SimulatorProcess() {
  // If we own members, we have to delete them.
  if (own_com_) {
    delete com_;
  }
}

void SimulatorProcess::Run() {
  // Perform set-up.
  if (!SetUp()) {
    return;
  }

  while (HandleSerialMessage());
}

bool SimulatorProcess::SetUp() {
  // First, open the com channel.
  return com_->Open();
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
