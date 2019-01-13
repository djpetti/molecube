#ifndef LIBMC_CORE_PROCESS_INTERFACE_H_
#define LIBMC_CORE_PROCESS_INTERFACE_H_

#include <memory>

namespace libmc {
namespace core {

// Common interface for all processes.
class ProcessInterface {
 public:
  virtual ~ProcessInterface() = default;

  // Runs the process indefinitely. This never exits. If it does exit, that
  // means there was some sort of unrecoverable failure.
  virtual void Run() = 0;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_PROCESS_H_
