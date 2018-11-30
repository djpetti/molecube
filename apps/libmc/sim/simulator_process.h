#ifndef LIBMC_SIM_SIMULATOR_PROCESS_H_
#define LIBMC_SIM_SIMULATOR_PROCESS_H_

#include "apps/libmc/core/events/event_dispatcher_interface.h"
#include "simulator_com_interface.h"

namespace libmc {
namespace sim {

// This process is designed to handle all of the direct interaction with the
// simulator.
class SimulatorProcess {
 public:
  SimulatorProcess();
  // Alternate constructor that allows for dependency injection during testing.
  // Args:
  //  com: The SimulatorCom to use.
  //  dispatcher: The EventDispatcher to use.
  SimulatorProcess(SimulatorComInterface *com,
                   core::events::EventDispatcherInterface *dispatcher);
  ~SimulatorProcess();

  // Runs the process indefinitely.
  // Returns:
  //  True if the process exited normally, false if it failed.
  bool Run();

 private:
  // Performs one-time setup.
  // Returns:
  //  True if setup succeeded, false otherwise.
  bool SetUp();
  // Handles the next message from the serial.
  // Returns:
  //  True if handling was successful, false otherwise.
  bool HandleSerialMessage();

  // SimulatorCom instance to use for communication.
  SimulatorComInterface *com_;
  // Whether we own the SimulatorCom.
  bool own_com_ = false;

  // EventDispatcher to use for dispatching system events.
  core::events::EventDispatcherInterface *dispatcher_;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_SIMULATOR_PROCESS_H_
