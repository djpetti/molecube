#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_DISPATCHER_H_

#include "gmock/gmock.h"

#include "google/protobuf/message_lite.h"

#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/proto_event_dispatcher_interface.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Basic mock class for all Protobuf event dispatchers.
class MockProtoEventDispatcher : public ProtoEventDispatcherInterface {
 public:
  MOCK_METHOD1(Dispatch, bool(EventCommon *event));
  MOCK_METHOD1(DispatchMessage,
               bool(const ::google::protobuf::MessageLite *message));
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif   // LIBMC_CORE_EVENTS_TESTS_MOCK_PROTO_EVENT_DISPATCHER_H_
