#include "system_event_dispatcher.h"

#include "apps/libmc/core/system_manager.h"

namespace libmc {
namespace core {
namespace events {

SystemEventDispatcher &SystemEventDispatcher::GetInstance() {
  // This is a slight abuse of the static keyword, but it makes our life easier
  // by guaranteeing that this will be created once and eventually destroyed.
  static SystemEventDispatcher instance;
  return instance;
}

SystemEventDispatcher::SystemEventDispatcher(SystemManagerInterface *manager)
    : manager_(manager) {}

SystemEventDispatcher::SystemEventDispatcher()
    : SystemEventDispatcher(new SystemManager) {
  // We own this manager.
  own_manager_ = true;
}

SystemEventDispatcher::~SystemEventDispatcher() {
  if (own_manager_) {
    // We have to destroy it.
    delete manager_;
  }
}

bool SystemEventDispatcher::Dispatch(EventCommon *event) {
  // First, cast to the proper event type.
  SystemEvent *sys_event = reinterpret_cast<SystemEvent *>(event);

  // Set the header fields.
  sys_event->Common.Type = EventType::SYSTEM;

  // TODO (danielp): Switch to queue-based infrastructure.
  if (sys_event->Shutdown) {
    return manager_->Shutdown();
  }

  return true;
}

bool SystemEventDispatcher::Dispatch(bool shutdown) {
  // Create the event.
  SystemEvent event;
  event.Shutdown = shutdown;

  // Dispatch it.
  return Dispatch(&(event.Common));
}

}  // namespace events
}  // namespace core
}  // namespace libmc
