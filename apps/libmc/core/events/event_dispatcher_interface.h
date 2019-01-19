#ifndef LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_

#include "event.h"

namespace libmc {
namespace core {
namespace events {

// Defines a common interface for all event dispatchers. An event dispatcher is
// responsible for creating a particular type of event and for routing it to the
// correct place.
class EventDispatcherInterface {
 public:
  virtual ~EventDispatcherInterface() = default;

  // Dispatches an event to the proper location. It will fill in the
  // header fields automatically.
  // Args:
  //  event: The event to dispatch. It will dynamically infer the size and
  //         format of the event structure based on the type.
  // Returns:
  //  True if the dispatch succeeded, false otherwise.
  virtual bool Dispatch(EventCommon *event) = 0;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
