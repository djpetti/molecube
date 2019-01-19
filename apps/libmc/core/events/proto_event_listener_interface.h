#ifndef LIBMC_CORE_EVENTS_PROTO_EVENT_LISTENER_INTERFACE_H_
#define LIBMC_CORE_EVENTS_PROTO_EVENT_LISTENER_INTERFACE_H_

#include "google/protobuf/message_lite.h"

#include "event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {

// An event listener interface that supports converting received events into
// Protobuf messages.
class ProtoEventListenerInterface : public EventListenerInterface {
 public:
  virtual ~ProtoEventListenerInterface() = default;

  // Listens for an event, and converts it to a Protobuf message.
  // Args:
  //  message: The message. It will be downcasted to an appropriate message
  //           type.
  virtual void ListenProtobuf(::google::protobuf::MessageLite *message) = 0;
  // Same as ListenProtobuf(), except non-blocking.
  // Args:
  //  message: The message. It will be downcasted to an appropriate message
  //           type.
  // Returns:
  //  True if an event was read, false if none was available.
  virtual bool GetProtobuf(::google::protobuf::MessageLite *message) = 0;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_PROTO_EVENT_LISTENER_INTERFACE_H_
