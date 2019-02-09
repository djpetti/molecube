#ifndef LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_MULTIPLEXER_H_
#define LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_MULTIPLEXER_H_

#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/event_multiplexer_interface.h"
#include "apps/libmc/core/events/event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {
namespace testing {

// Mock event listener class.
class MockEventMultiplexer : public EventMultiplexerInterface {
 public:
  MOCK_METHOD2(AddListener,
               void(EventListenerInterface *listener, EventType type));
  MOCK_METHOD1(Select, EventListenerInterface *(EventType *type));
};

}  // namespace testing
}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_TESTS_MOCK_EVENT_MULTIPLEXER_H_
