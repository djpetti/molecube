#ifndef LIBMC_CORE_SYSTEM_MANAGER_H_
#define LIBMC_CORE_SYSTEM_MANAGER_H_

namespace libmc {
namespace core {

// Defines functionality for managing the system at a high level.
class SystemManager {
 public:
  // Instructs the cube to shut down immediately.
  // Returns:
  //  True if it succeeded, false otherwise.
  bool ShutDown();
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_SYSTEM_MANAGER_H_
