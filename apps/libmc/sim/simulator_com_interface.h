#ifndef LIBMC_SIM_SIMULATOR_COM_INTERFACE_H_
#define LIBMC_SIM_SIMULATOR_COM_INTERFACE_H_

#include "google/protobuf/message_lite.h"

namespace libmc {
namespace sim {

typedef ::google::protobuf::MessageLite ProtoMessage;

// Interface for the SimulatorCom class.
class SimulatorComInterface {
 public:
  virtual ~SimulatorComInterface() = default;

  // Opens a new connection on the simulator serial port.
  // Returns:
  //  True if it succeeded in opening the connection, false otherwise.
  virtual bool Open() = 0;

  // Sends a message to the simulator.
  // Args:
  //  message: The message to send.
  // Returns:
  //  True if sending the message succeeded, false otherwise.
  virtual bool SendMessage(const ProtoMessage *message) = 0;
  // Receives a message from the simulator.
  // Args:
  //  message: The message to receive into.
  // Returns:
  //  True if receiving the message succeeded, false otherwise.
  virtual bool ReceiveMessage(ProtoMessage *message) = 0;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_SIMULATOR_COM_INTERFACE_H_
