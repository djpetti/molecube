#include "system_event_dispatcher.h"

#include <assert.h>

#include "glog/logging.h"

#include "tachyon/lib/queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/sim/protobuf/system_message.pb.h"

namespace libmc {
namespace core {
namespace events {

using ::tachyon::QueueInterface;
using ::tachyon::Queue;

SystemEventDispatcher &SystemEventDispatcher::GetInstance() {
  VLOG(1) << "Fetching queue: " << constants::kQueueNames.SysManagerQueue;

  // This is a slight abuse of the static keyword, but it makes our life easier
  // by guaranteeing that this will be created once and eventually destroyed.
  static const ::std::unique_ptr<QueueInterface<SystemEvent>> &queue =
      Queue<SystemEvent>::FetchProducerQueue(
          constants::kQueueNames.SysManagerQueue);
  static SystemEventDispatcher instance(queue);
  return instance;
}

SystemEventDispatcher &SystemEventDispatcher::CreateWithQueue(
    const ::std::unique_ptr<QueueInterface<SystemEvent>> &queue) {
  static SystemEventDispatcher instance(queue);
  return instance;
}

SystemEventDispatcher::SystemEventDispatcher(
    const ::std::unique_ptr<QueueInterface<SystemEvent>> &queue)
    : queue_(queue) {}

bool SystemEventDispatcher::Dispatch(EventCommon *event) {
  // First, cast to the proper event type.
  SystemEvent *sys_event = reinterpret_cast<SystemEvent *>(event);

  // Set the header fields.
  sys_event->Common.Type = EventType::SYSTEM;

  // Send the message.
  if (!queue_->EnqueueBlocking(*sys_event)) {
    // This really shouldn't fail unless we don't have a consumer, which would
    // imply that the system manager process died.
    LOG(ERROR) << "Enqueue failed. Is system_manager_process running?";
    return false;
  }

  return true;
}

bool SystemEventDispatcher::Dispatch(bool shutdown) {
  // Create the event.
  SystemEvent event;
  event.Shutdown = shutdown;

  // Dispatch it.
  return Dispatch(&(event.Common));
}

bool SystemEventDispatcher::DispatchMessage(
    const ::google::protobuf::MessageLite *message) {
  // Downcast to the correct message type.
  const sim::SystemMessage *sys_message =
      dynamic_cast<const sim::SystemMessage *>(message);
  // Check that a valid message was passed.
  assert(sys_message);

  // Dispatch an event based on the contents.
  return Dispatch(sys_message->shutdown());
}

}  // namespace events
}  // namespace core
}  // namespace libmc
