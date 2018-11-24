#include "system_manager.h"

#include <linux/reboot.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "glog/logging.h"

namespace libmc {
namespace core {

bool SystemManager::Shutdown() {
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
