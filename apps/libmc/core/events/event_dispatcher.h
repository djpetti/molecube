#ifndef LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_

#include "event.h"

namespace libmc {
namespace core {
namespace events {

// Defines common functionality for all event dispatchers. An event dispatcher is
// responsible for creating a particular type of event and for routing it to the
// correct place.
class EventDispatcher {
 public:
  EventDispatcher() = default;
  virtual ~EventDispatcher() = default;

  // Dispatches an event to the proper location. It will fill in the
  // header fields automatically.
  // Args:
  //  event: The event to dispatch. It will dynamically infer the size and
  //         format of the event structure based on the type.
  // Returns:
  //  True if the dispatch succeeded, false otherwise.
  virtual bool Dispatch(EventCommon *event) = 0;

 protected:
  // Allows an EventDispatcher to be constructed in such a way that overrides
  // the default specifications for which events are multiplexed and which are
  // not.
  // Args:
  //  multiplexed: If true, it will treat the event being dispatched as being
  //               multiplexed, if false, it won't.
  explicit EventDispatcher(bool multiplexed);

  // Checks if the event being dispatched is multiplexed, and if so, performs
  // the necessary preparation before dispatch. This should generally be called
  // at some point in every dispatch method.
  // Args:
  //  type: The type of event being dispatched.
  // Returns:
  //  True if it succeeded, false otherwise.
  bool PrepareMultiplexed(EventType type);

 private:
  // True if multiplex settings are being overriden, false otherwise.
  bool multiplex_override_ = false;
  // If overriden, specifies whether we should or shouldn't assume multiplexing.
  bool multiplexed_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
