#ifndef LIBMC_CORE_EVENTS_SYSTEM_EVENT_H_
#define LIBMC_CORE_EVENTS_SYSTEM_EVENT_H_

#include "event.h"

namespace libmc {
namespace core {
namespace events {

// System event. This event handles commands to the base system.
struct SystemEvent {
  EventCommon Common;

  // Whether we want to shutdown the system.
  bool Shutdown;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_SYSTEM_EVENT_H_
