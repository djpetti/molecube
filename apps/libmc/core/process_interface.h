#ifndef LIBMC_CORE_PROCESS_INTERFACE_H_
#define LIBMC_CORE_PROCESS_INTERFACE_H_

#include <memory>

namespace libmc {
namespace core {

// Common interface for all processes.
class ProcessInterface {
 public:
  virtual ~ProcessInterface() = default;

  // Runs a single iteration of the process. This may block waiting for stuff to
  // do.
  // Returns:
  //  True if running it succeeded, false otherwise.
  virtual bool RunIteration() = 0;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_PROCESS_H_
