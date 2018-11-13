#ifndef LIBMC_SIM_SIMULATOR_COM_H_
#define LIBMC_SIM_SIMULATOR_COM_H_

#include "google/protobuf/message_lite.h"

#include "cows_interface.h"
#include "serial_link_interface.h"

namespace libmc {
namespace sim {

typedef ::google::protobuf::MessageLite ProtoMessage;

// Defines a high-level interface for communicating with the simulation.
class SimulatorCom {
 public:
  SimulatorCom();
  // Allows us to inject the SerialLink and Cows to use when unit testing.
  // Args:
  //  serial_link: The serial link object to use.
  //  cows: The Cows object to use.
  SimulatorCom(SerialLinkInterface *serial_link, CowsInterface *cows);
  ~SimulatorCom();

  // Opens a new connection on the simulator serial port.
  // Returns:
  //  True if it succeeded in opening the connection, false otherwise.
  bool Open();

  // Sends a message to the simulator.
  // Args:
  //  message: The message to send.
  // Returns:
  //  True if sending the message succeeded, false otherwise.
  bool SendMessage(const ProtoMessage &message);
  // Receives a message from the simulator.
  // Args:
  //  message: The message to receive into.
  // Returns:
  //  True if receiving the message succeeded, false otherwise.
  bool ReceiveMessage(ProtoMessage *message);

 private:
  // Reads from the serial port until it finds a valid packet separator.
  // Returns:
  //  True if it successfully found the separator, false if a read error
  //  occurred.
  bool SyncToPacket();
  // Finds the end of the first packet in the buffer.
  // Args:
  //  start: Where to start searching in the buffer.
  // Returns:
  //  The index of byte after the end of the packet, or 0 if there is no
  //  complete packet.
  uint32_t FindPacketEnd(uint32_t start);
  // Removes the first packet in the buffer, and shifts everything after it
  // down.
  // Args:
  //  packet_end: The index of the byte after the end of the first packet.
  void ClearPacket(uint32_t packet_end);

  // Internal buffer that we use for storing serialized messages to be sent.
  uint8_t *send_buffer_;
  // Internal buffer that we use for storing received messages.
  uint8_t *receive_buffer_;
  // Whether we're synced to packets.
  bool packet_synced_ = false;
  // Amount of space used in the receive buffer.
  uint32_t receive_space_used_ = 0;

  // Internal low-level serial interface.
  SerialLinkInterface *serial_;
  // Whether we have ownership of the serial link.
  bool own_serial_ = false;

  // Internal COWS implementation.
  CowsInterface *cows_;
  // Whether we have ownership of cows.
  bool own_cows_ = false;
};

}  // namespace sim
}  // namespace libmc

#endif  // LIBMC_SIM_SIMULATOR_COM_H_
