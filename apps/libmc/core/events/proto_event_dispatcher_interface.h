#ifndef LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_INTERFACE_H_

#include "google/protobuf/message_lite.h"

#include "event_dispatcher_interface.h"
#include "event.h"

namespace libmc {
namespace core {
namespace events {

// Defines a common interface for all event dispatchers that are capable of
// accepting Protobuf messages directly.
class ProtoEventDispatcherInterface : public EventDispatcherInterface {
 public:
  virtual ~ProtoEventDispatcherInterface() = default;

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

#endif  // LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_INTERFACE_H_
