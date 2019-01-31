#ifndef LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_INTERFACE_H_

#include "event.h"
#include "event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {

// Handles multiplexing multiple event listeners together, so we can listen to
// all of them at once from a single thread.
class EventMultiplexerInterface {
 public:
  virtual ~EventMultiplexerInterface() = default;

  // Adds a new event listener to the multiplexer.
  // Args:
  //  listener: The EventListener to add.
  //  type: The type of event it is listening for.
  virtual void AddListener(EventListenerInterface *listener,
                           EventType type) = 0;
  // Waits until one of the listeners has a new event.
  // Args:
  //  type: Set to the type of event that the listener will receive.
  // Returns:
  //  The listener that has a new event.
  virtual EventListenerInterface *Select(EventType *type) = 0;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif // LIBMC_CORE_EVENTS_EVENT_MULTIPLEXER_INTERFACE_H_
