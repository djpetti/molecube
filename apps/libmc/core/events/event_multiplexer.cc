#include "event_multiplexer.h"

#include <utility>

#include "glog/logging.h"

#include "apps/libmc/constants.h"

namespace libmc {
namespace core {
namespace events {

using constants::kQueueNames;
using ::tachyon::Queue;

void EventMultiplexer::PrepareDispatch(EventType type) {
  // It would be ideal if this were a static class member, but having a queue as
  // one wreaks havok with the unit tests.
  static const EventMultiplexer::NotificationQueue notification_producer_queue =
      Queue<EventNotification>::FetchProducerQueue(
          kQueueNames.EventNotifyQueue);

  // Create and send the notification.
  EventNotification notification = {type};
  notification_producer_queue->EnqueueBlocking(notification);
}

EventMultiplexer::EventMultiplexer()
    : notification_queue_(
          Queue<EventNotification>::FetchQueue(kQueueNames.EventNotifyQueue)) {}

EventMultiplexer::EventMultiplexer(NotificationQueue &queue) {
  notification_queue_ = ::std::move(queue);
}

void EventMultiplexer::AddListener(EventListenerInterface *listener,
                                   EventType type) {
  // Recored the listener.
  listeners_[type] = listener;
}

EventListenerInterface *EventMultiplexer::Select(EventType *type) {
  // First, wait for an event notification.
  EventNotification notification;

  while (true) {
    notification_queue_->DequeueNextBlocking(&notification);

    VLOG(1) << "Got new event notification.";

    // Get the corresponding listener.
    const auto &listener = listeners_.find(notification.Type);
    if (listener == listeners_.end()) {
      // We don't have a listener to handle this event type. We can ignore it.
      VLOG(1) << "Ignoring event of unregistered type.";
      continue;
    }

    *type = notification.Type;
    return listener->second;
  }
}

}  // namespace events
}  // namespace core
}  // namespace libmc
