#ifndef LIBMC_SIM_SIMULATOR_PROCESS_H_
#define LIBMC_SIM_SIMULATOR_PROCESS_H_

#include "apps/libmc/core/events/proto_event_dispatcher.h"
#include "apps/libmc/core/process_interface.h"
#include "simulator_com_interface.h"

namespace libmc {
namespace sim {

// This process is designed to handle all of the direct interaction with the
// simulator.
class SimulatorProcess : public core::ProcessInterface {
 public:
  SimulatorProcess();
  // Alternate constructor that allows for dependency injection during testing.
  // Args:
  //  com: The SimulatorCom to use.
  //  dispatcher: The EventDispatcher to use.
  SimulatorProcess(SimulatorComInterface *com,
                   core::events::ProtoEventDispatcher *dispatcher);
  ~SimulatorProcess();

  virtual void Run();

 private:
  // Performs one-time setup.
  // Returns:
  //  True if setup succeeded, false otherwise.
  bool SetUp();

  // Handles the next message from the serial.
  // Returns:
  //  True if handling was successful, false otherwise.
  bool HandleSerialMessage();
  // Handles the next event.
  // Returns:
  //  True if handling was successful, false otherwise.
  bool HandleEvent();

  // Entry point for thread receiving serial data.
  void ReceivingThread();
  // Entry point for thread sending serial data.
  void SendingThread();

  // SimulatorCom instance to use for communication.
  SimulatorComInterface *com_;
  // Whether we own the SimulatorCom.
  bool own_com_ = false;

  // EventDispatcher to use for dispatching system events.
  core::events::ProtoEventDispatcher *dispatcher_;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_SIMULATOR_PROCESS_H_
