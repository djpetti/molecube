#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_LISTENER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_LISTENER_H_

#include "apps/libmc/core/events/event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Mock event listener class.
class MockEventListener : public EventListenerInterface {
 public:
  MOCK_METHOD1(Listen, void(EventCommon *event));
  MOCK_METHOD1(Get, bool(EventCommon *event));
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_LISTENER_H_
