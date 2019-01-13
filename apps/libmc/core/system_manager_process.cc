#include "system_manager_process.h"

#include <linux/reboot.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "glog/logging.h"

namespace libmc {
namespace core {

using events::SystemEvent;

SystemManagerProcess::SystemManagerProcess(
    const ::std::unique_ptr<SystemEventQueueType> &queue)
    : queue_(queue) {}

void SystemManagerProcess::Run() {
  while (RunIteration());
}

bool SystemManagerProcess::RunIteration() {
  // Receive the next message from the queue.
  SystemEvent event;
  queue_->DequeueNextBlocking(&event);
  VLOG(1) << "Received new message.";

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
