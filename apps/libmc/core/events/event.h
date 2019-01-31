#ifndef LIBMC_CORE_EVENTS_EVENT_H_
#define LIBMC_CORE_EVENTS_EVENT_H_

#include <unordered_map>

namespace libmc {
namespace core {
namespace events {

// Enum defines all known event types.
enum class EventType {
  // System event.
  SYSTEM,
  // Graphics event.
  GRAPHICS,
};

// Specifies which types of events will be used with a multiplexer. Set to true
// if that event type will be used with a multiplexer.
const ::std::unordered_map<EventType, bool> kIsMultiplexed = {
    {EventType::SYSTEM, false}, {EventType::GRAPHICS, true}};

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
