#ifndef LIBMC_CORE_LINUX_MANAGEMENT_H_
#define LIBMC_CORE_LINUX_MANAGEMENT_H_

#include "linux_management_interface.h"

namespace libmc {
namespace core {

// Standard implementation for LinuxManagementInterface.
class LinuxManagement : public LinuxManagementInterface {
 public:
  // Since everything here is stateless, we really only need one of these
  // floating around. Therefore, this method gets the singleton instance,
  // creating it first if necessary.
  static const LinuxManagement &GetInstance();

  // Disable copy and assignment.
  LinuxManagement (LinuxManagement const &other) = delete;
  void operator=(LinuxManagement const &other) = delete;

  virtual void Sync() const;
  virtual bool Halt() const;

 private:
  // Private constructor to enfore singleton pattern.
  LinuxManagement() = default;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_LINUX_MANAGEMENT_H_
