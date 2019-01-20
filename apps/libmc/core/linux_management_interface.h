#ifndef LIBMC_CORE_LINUX_MANAGEMENT_INTERFACE_H_
#define LIBMC_CORE_LINUX_MANAGEMENT_INTERFACE_H_

namespace libmc {
namespace core {

// Defines an interface for Linux system management operations. This exists so
// that mocks can be easily written.
class LinuxManagementInterface {
 public:
  // Writes any data buffered in memory out to disk.
  virtual void Sync() const = 0;
  // Causes the system to halt immediately.
  // Returns:
  //  Does not return if it succeeds, and returns false if it fails.
  virtual bool Halt() const = 0;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_LINUX_MANAGEMENT_INTERFACE_H_
