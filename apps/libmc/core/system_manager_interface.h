#ifndef LIBMC_CORE_SYSTEM_MANAGER_INTERFACE_H_
#define LIBMC_CORE_SYSTEM_MANAGER_INTERFACE_H_

namespace libmc {
namespace core {

// Interface for the SystemManager class.
class SystemManagerInterface {
 public:
  virtual ~SystemManagerInterface() = default;

  // Instructs the cube to shut down immediately.
  // Returns:
  //  True if it succeeded, false otherwise.
  virtual bool Shutdown() = 0;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_SYSTEM_MANAGER_INTERFACE_H_
