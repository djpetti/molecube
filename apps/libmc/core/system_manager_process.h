#ifndef LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_
#define LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_

#include <memory>

#include "tachyon/lib/queue.h"

#include "apps/libmc/core/events/system_event.h"
#include "process_interface.h"

namespace libmc {
namespace core {

// Process that handles managing the system at a high level.
class SystemManagerProcess : public ProcessInterface {
 public:
  typedef ::tachyon::Queue<events::SystemEvent> SystemEventQueueType;

  // Args:
  //  queue: The queue to receive events on.
  SystemManagerProcess(const ::std::unique_ptr<SystemEventQueueType> &queue);
  virtual ~SystemManagerProcess() = default;

  virtual bool RunIteration();

 private:
  // Shuts down the cube.
  // Returns:
  //  When this works, it never returns. Otherwise, it should return false on
  //  failure.
  bool Shutdown();

  // Queue that we receive events on.
  const ::std::unique_ptr<SystemEventQueueType> &queue_;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_
