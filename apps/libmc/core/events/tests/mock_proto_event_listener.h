#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_LISTENER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_LISTENER_H_

#include "google/protobuf/message_lite.h"

#include "apps/libmc/core/events/proto_event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Mock event proto event listener class.
class MockProtoEventListener : public ProtoEventListenerInterface {
 public:
  MOCK_METHOD1(Listen, void(EventCommon *event));
  MOCK_METHOD1(Get, bool(EventCommon *event));

  MOCK_METHOD1(ListenProtobuf, void(::google::protobuf::MessageLite *message));
  MOCK_METHOD1(GetProtobuf, bool(::google::protobuf::MessageLite *message));
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_LISTENER_H_
