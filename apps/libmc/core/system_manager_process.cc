#include "system_manager_process.h"

#include <linux/reboot.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "glog/logging.h"

#include "apps/libmc/core/events/event.h"
#include "apps/libmc/core/events/system_event.h"

namespace libmc {
namespace core {

using events::EventListenerInterface;
using events::EventType;
using events::SystemEvent;

SystemManagerProcess::SystemManagerProcess(EventListenerInterface *listener)
    : listener_(CHECK_NOTNULL(listener)) {}

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
  sync();

  if (reboot(LINUX_REBOOT_CMD_POWER_OFF) < 0) {
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
