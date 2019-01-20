#include "system_manager_process.h"

#include "glog/logging.h"

#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/system_event.h"
#include "apps/libmc/core/events/system_event_listener.h"
#include "linux_management.h"

namespace libmc {
namespace core {

using events::EventListenerInterface;
using events::EventType;
using events::SystemEvent;
using events::SystemEventListener;

SystemManagerProcess::SystemManagerProcess()
    : SystemManagerProcess(&SystemEventListener::GetInstance(),
                           &LinuxManagement::GetInstance()) {}

SystemManagerProcess::SystemManagerProcess(
    EventListenerInterface *listener, const LinuxManagementInterface *linux)
    : listener_(CHECK_NOTNULL(listener)), linux_(CHECK_NOTNULL(linux)) {}

void SystemManagerProcess::Run() {
  while (RunIteration());
}

bool SystemManagerProcess::RunIteration() {
  // Receive the next message from the queue.
  SystemEvent event;
  listener_->Listen(&(event.Common));
  // Make sure the event is actually of the right type.
  DLOG_IF(FATAL, event.Common.Type != EventType::SYSTEM)
      << "Expected SystemEvent.";

  bool success = true;

  // Perform the required action.
  if (event.Shutdown) {
    success = Shutdown();
  }

  return success;
}

bool SystemManagerProcess::Shutdown() {
  LOG(INFO) << "System will now reboot.";

  // Flush any buffers to disk.
  linux_->Sync();

  if (!linux_->Halt()) {
    // Failure to halt.
    PLOG(ERROR) << "Failed to halt";
    return false;
  }

  // It should halt before it actually gets here.
  LOG(WARNING) << "reboot() exiting normally?";
  return true;
}

}  // namespace core
}  // namespace libmc
