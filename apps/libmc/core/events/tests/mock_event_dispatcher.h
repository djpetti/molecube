#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_DISPATCHER_H_

#include "gmock/gmock.h"

#include "apps/libmc/core/events/event_dispatcher.h"
#include "apps/libmc/core/events/event.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Basic mock class for all event dispatchers.
class MockEventDispatcher : public EventDispatcher {
 public:
  MOCK_METHOD1(Dispatch, bool(EventCommon *event));
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif   // LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_DISPATCHER_H_
