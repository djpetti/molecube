#include "system_event_listener.h"

#include "glog/logging.h"

#include "tachyon/lib/queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/sim/protobuf/system_message.pb.h"

namespace libmc {
namespace core {
namespace events {

using ::tachyon::Queue;

SystemEventListener &SystemEventListener::GetInstance() {
  VLOG(1) << "Fetching queue: " << constants::kQueueNames.SysManagerQueue;

  // This is a slight abuse of the static keyword, but it makes our life easier
  // by guaranteeing that this will be created once and eventually destroyed.
  static const QueuePtr &queue =
      Queue<SystemEvent>::FetchQueue(constants::kQueueNames.SysManagerQueue);
  static SystemEventListener instance(queue);
  return instance;
}

SystemEventListener &SystemEventListener::CreateWithQueue(
    const QueuePtr &queue) {
  static SystemEventListener instance(queue);
  return instance;
}

SystemEventListener::SystemEventListener(const QueuePtr &queue)
    : queue_(queue) {}

void SystemEventListener::Listen(EventCommon *event) {
  DLOG_IF(FATAL, !event) << "'event' should not be nullptr.";
  // Assume the event is of the correct type.
  SystemEvent *sys_event = reinterpret_cast<SystemEvent *>(event);

  // Wait for the next event.
  queue_->DequeueNextBlocking(sys_event);
  VLOG(1) << "Received new event.";
  DLOG_IF(FATAL, sys_event->Common.Type != EventType::SYSTEM)
      << "Did not get system event?";
}

bool SystemEventListener::Get(EventCommon *event) {
  DLOG_IF(FATAL, !event) << "'event' should not be nullptr.";
  // Assume the event is of the correct type.
  SystemEvent *sys_event = reinterpret_cast<SystemEvent *>(event);

  // Try getting the next event.
  if (!queue_->DequeueNext(sys_event)) {
    // No available events.
    return false;
  }

  VLOG(1) << "Received new event.";
  DLOG_IF(FATAL, sys_event->Common.Type != EventType::SYSTEM)
      << "Did not get system event?";

  return true;
}

}  // namespace events
}  // namespace core
}  // namespace libmc
