#include "linux_management.h"

#include <sys/reboot.h>
#include <unistd.h>

namespace libmc {
namespace core {

const LinuxManagement &LinuxManagement::GetInstance() {
  static const LinuxManagement instance;
  return instance;
}

void LinuxManagement::Sync() const {
  sync();
}

bool LinuxManagement::Halt() const {
  return reboot(RB_POWER_OFF) >= 0;
}

}  // namespace core
}  // namespace libmc
