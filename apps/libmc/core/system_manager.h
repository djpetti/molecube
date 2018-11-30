#ifndef LIBMC_CORE_SYSTEM_MANAGER_H_
#define LIBMC_CORE_SYSTEM_MANAGER_H_

#include "system_manager_interface.h"

namespace libmc {
namespace core {

// Defines functionality for managing the system at a high level.
class SystemManager : public SystemManagerInterface {
 public:
  virtual ~SystemManager() = default;

  virtual bool Shutdown();
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_SYSTEM_MANAGER_H_
