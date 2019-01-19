#ifndef LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_
#define LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_

#include "apps/libmc/core/events/event_listener_interface.h"
#include "process_interface.h"

namespace libmc {
namespace core {

// Process that handles managing the system at a high level.
class SystemManagerProcess : public ProcessInterface {
 public:
  // Args:
  //  listener: The EventListener to listen for SystemEvents on.
  SystemManagerProcess(events::EventListenerInterface *listener);
  virtual ~SystemManagerProcess() = default;

  virtual void Run();

 private:
  // Shuts down the cube.
  // Returns:
  //  When this works, it never returns. Otherwise, it should return false on
  //  failure.
  bool Shutdown();
  // Runs a single iteration of the process.
  // Returns:
  //  True if running this iteration succeeded, false otherwise.
  bool RunIteration();

  // Listener that we use to listen for system events.
  events::EventListenerInterface *listener_;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_SYSTEM_MANAGER_PROCESS_H_
