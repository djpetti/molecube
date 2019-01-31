#ifndef LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_H_
#define LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_H_

#include <memory>
#include <unordered_map>

#include "tachyon/lib/queue.h"
#include "tachyon/lib/queue_interface.h"

#include "event.h"
#include "event_listener_interface.h"
#include "event_multiplexer_interface.h"

namespace libmc {
namespace core {
namespace events {

class EventMultiplexer : public EventMultiplexerInterface {
 public:
  // A structure to send through the event notification queue. Internally, the
  // multiplexer sends these messages on a single, special queue whenever an
  // event is dispatched.
  struct EventNotification {
    // The type of the event being dispatched.
    EventType Type;
  };

  // Typedef for the notification queue.
  typedef ::std::unique_ptr<::tachyon::QueueInterface<EventNotification>>
      NotificationQueue;

  // Prepares for a new event to be dispatched on a multiplexed queue. This
  // must be run during the Dispatch() method of dispatchers for multiplexed
  // events.
  // Args:
  //  type: The type of the event being dispatched.
  // Returns:
  //  True if it succeeded, false otherwise.
  static bool PrepareDispatch(EventType type);

  EventMultiplexer();
  // Alternate constructor that allows for injection of the queue, intended for
  // testing.
  // Args:
  //  queue: The queue to listen for event notifications one.
  explicit EventMultiplexer(NotificationQueue &queue);

  // Disable copying and assignment, which is important since we have a
  // unique_ptr internally.
  EventMultiplexer(EventMultiplexer const &other) = delete;
  EventMultiplexer &operator=(EventMultiplexer const &other) = delete;

  virtual void AddListener(EventListenerInterface *listener, EventType type);
  virtual EventListenerInterface *Select(EventType *type);

 private:
  // A separate handle to the same queue for receiving items. This allows us to
  // have one handle per instance.
  NotificationQueue notification_queue_;
  // Maps event types to the listener for that type of event.
  ::std::unordered_map<EventType, EventListenerInterface *> listeners_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif // LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_H_
