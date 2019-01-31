#include "event_dispatcher.h"

#include "glog/logging.h"

#include "event_multiplexer.h"

namespace libmc {
namespace core {
namespace events {

EventDispatcher::EventDispatcher(bool multiplexed)
    : multiplex_override_(true), multiplexed_(multiplexed) {}

bool EventDispatcher::PrepareMultiplexed(EventType type) {
  // Figure out whether this event is multiplexed or not.
  const auto &should_multiplex = kIsMultiplexed.find(type);
  DLOG_IF(FATAL, should_multiplex == kIsMultiplexed.end())
      << "Forgot to add type to kIsMultiplexed?";

  if ((multiplex_override_ && multiplexed_) ||
      (!multiplex_override_ && should_multiplex->second)) {
    // Prepare for multiplexing.
    VLOG(2) << "Preparing for multiplexing.";

    return EventMultiplexer::PrepareDispatch(type);
  }

  return true;
}

}  // namespace events
}  // namespace core
}  // namespace libmc
