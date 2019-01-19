#ifndef LIBMC_CORE_EVENTS_EVENT_LISTENER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_EVENT_LISTENER_INTERFACE_H_

#include "event.h"

namespace libmc {
namespace core {
namespace events {

// Defines a common interface for all event listeners. An event listener is
// responsible for reading a particular event type from the correct queue.
class EventListenerInterface {
 public:
  virtual ~EventListenerInterface() = default;

  // Listens for an event indefinitely.
  // Args:
  //  event: Will be filled with the event that was read.
  //         It will dynamically infer the size and format of the
  //         event structure based on the type.
  virtual void Listen(EventCommon *event) = 0;
  // Same as Listen(), except non-blocking.
  // Args:
  //  event: Will be filled with the latest event that was read. It will
  //         dynamically infer the size and format of the event structure based
  //         on the type.
  // Returns:
  //  True if an event was read, false if none was available.
  virtual bool Get(EventCommon *event) = 0;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_EVENT_LISTENER_INTERFACE_H_
