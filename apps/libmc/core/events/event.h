#ifndef LIBMC_CORE_EVENTS_EVENT_H_
#define LIBMC_CORE_EVENTS_EVENT_H_

namespace libmc {
namespace core {
namespace events {

// Enum defines all known event types.
enum class EventType {
  // System event.
  SYSTEM,
};

// Defines the common part of all events. This MUST ALWAYS be the first item in
// any event structure.
struct EventCommon {
  // The type of the event.
  EventType Type;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_EVENT_H_
