#ifndef LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_

#include "google/protobuf/message_lite.h"

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

  // Dispatches an event to the proper location. Eventually, this will use
  // queues, but right now it does the dispatch directly. It will fill in the
  // header fields automatically.
  // Args:
  //  event: The event to dispatch. It will dynamically infer the size and
  //         format of the event structure based on the type.
  // Returns:
  //  True if the dispatch succeeded, false otherwise.
  virtual bool Dispatch(EventCommon *event) = 0;
  // Possibly dispatches an event based on a received message. It is up to the
  // caller to ensure that a valid message of the right type is passed.
  // Args:
  //  message: The message to dispatch based on.
  // Returns:
  //  True if the dispatch succeeded or didn't happen, false if it failed.
  virtual bool DispatchMessage(
      const ::google::protobuf::MessageLite *message) = 0;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_EVENT_DISPATCHER_INTERFACE_H_
