#include "system_manager.h"

#include <linux/reboot.h>
#include <unistd.h>
#include <sys/reboot.h>

namespace libmc {
namespace core {

bool SystemManager::ShutDown() {
  // Flush any buffers to disk.
  sync();

  if (reboot(LINUX_REBOOT_CMD_HALT) < 0) {
    // Failure to halt.
    return false;
  }

  return true;
}

}  // namespace core
}  // namespace libmc
