#ifndef LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_H_

#include "google/protobuf/message_lite.h"

#include "event_dispatcher.h"
#include "event.h"

namespace libmc {
namespace core {
namespace events {

// Defines common functionality for all event dispatchers that are capable of
// accepting Protobuf messages directly.
class ProtoEventDispatcher : public EventDispatcher {
 public:
  ProtoEventDispatcher() = default;
  virtual ~ProtoEventDispatcher() = default;

  // Possibly dispatches an event based on a received message. It is up to the
  // caller to ensure that a valid message of the right type is passed.
  // Args:
  //  message: The message to dispatch based on.
  // Returns:
  //  True if the dispatch succeeded, false if it failed or didn't happen.
  virtual bool DispatchMessage(
      const ::google::protobuf::MessageLite *message) = 0;

 protected:
  // Equivalent constructor to the one for EventDispatcher() that allows us to
  // override multiplexing settings for testing.
  // Args:
  //  multiplexed: If true, multiplexing will always be enabled. If false, it
  //               will always be disabled.
  ProtoEventDispatcher(bool multiplexed) : EventDispatcher(multiplexed) {}
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_PROTO_EVENT_DISPATCHER_H_
