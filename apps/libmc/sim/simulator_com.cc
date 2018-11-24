#include "simulator_com.h"

#include <string.h>

#include "glog/logging.h"

#include "apps/libmc/constants.h"
#include "cows.h"
#include "serial_link.h"

namespace libmc {
namespace sim {

SimulatorCom::SimulatorCom()
    : SimulatorCom(new SerialLink(&backend_), new Cows) {
  // Indicate that we own this serial link and cows implementation.
  own_serial_ = true;
  own_cows_ = true;
}

SimulatorCom::SimulatorCom(SerialLinkInterface *serial_link,
                           CowsInterface *cows)
    : serial_(serial_link), cows_(cows) {
  // Allocate the buffer memory.
  send_buffer_ = new uint8_t[constants::kSimulator.MaxPacketSize];
  receive_buffer_ = new uint8_t[constants::kSimulator.MaxPacketSize];
}

SimulatorCom::~SimulatorCom() {
  // Free the serial link and Cows if we own them.
  if (own_serial_) {
    delete serial_;
  }
  if (own_cows_) {
    delete cows_;
  }

  // Free the buffer memory.
  delete[] send_buffer_;
  delete[] receive_buffer_;
}

bool SimulatorCom::Open() {
  // Open the underlying serial connections.
  const char *device = constants::kSimulator.Device;
  const uint32_t baud = constants::kSimulator.BaudRate;

  if (!serial_->Open(device, baud)) {
    return false;
  }

  // Send the first zero separator so the recipient knows a message is coming.
  VLOG(1) << "Sending zero separator.";
  const uint8_t zero[] = {0, 0};
  return serial_->SendMessage(zero, 2);
}

bool SimulatorCom::SendMessage(const ProtoMessage &message) {
  LOG(INFO) << "Sending message.";

  const uint32_t length = message.ByteSizeLong();
  // COWS adds 4 bytes of overhead.
  const uint32_t cows_length = length + 4;
  // We need the length to be a multiple of words, because that's how we stuff
  // it.
  const uint32_t padded_length = cows_length + cows_length % 2;
  VLOG(1) << "length: " << length << " padded_length: " << padded_length
          << "cows_length: " << cows_length;

  // Check that the message fits in the buffer.
  CHECK(padded_length < constants::kSimulator.MaxPacketSize);

  // Serialize the message.
  if (!message.SerializeToArray(send_buffer_ + 2, length)) {
    LOG(ERROR) << "Failed to serialize message.";
    return false;
  }

  // Stuff the message. (This uses the first word for the overhead.)
  cows_->CowsStuff(reinterpret_cast<uint16_t *>(send_buffer_),
                   padded_length / 2);

  // Add the zero separator at the end. (We don't need the padding)
  send_buffer_[cows_length - 2] = 0;
  send_buffer_[cows_length - 1] = 0;

  // Send the message on the wire.
  return serial_->SendMessage(send_buffer_, cows_length);
}

bool SimulatorCom::ReceiveMessage(ProtoMessage *message) {
  VLOG(1) << "Starting ReceiveMessage.";

  if (!packet_synced_) {
    // First, sync to the start of the packet.
    if (!SyncToPacket()) {
      return false;
    }
    packet_synced_ = true;
  }

  // Now, receive the rest of the packet into the buffer.
  const uint32_t max_length = constants::kSimulator.MaxPacketSize;
  uint32_t search_start = 0;
  while (!FindPacketEnd(search_start)) {
    if (receive_space_used_ == max_length) {
      // We've filled up our entire buffer and still don't have a packet.
      // Something is wrong.
      LOG(ERROR)
          << "Buffer is full, but no valid packet. Check max packet size.";

      receive_space_used_ = 0;
      packet_synced_ = false;
      return false;
    }

    VLOG(1) << "Looking for next partial message.";
    const int32_t bytes_read =
        serial_->ReceivePartialMessage(receive_buffer_ + receive_space_used_,
                                       max_length - receive_space_used_);
    VLOG(1) << "Received " << bytes_read << " bytes.";
    if (bytes_read < 0) {
      // Reading failure.
      receive_space_used_ = 0;
      packet_synced_ = false;
      return false;
    }

    // Since the separator is two bytes instead of one, we actually have to
    // overlap slightly with the previous search area when looking for it.
    if (receive_space_used_ != 0) {
      search_start = receive_space_used_ - 1;
    }
    receive_space_used_ += bytes_read;
    VLOG(1) << "Used " << receive_space_used_ << " bytes in buffer.";
  }

  // Parse the message.
  const uint32_t packet_end = FindPacketEnd(search_start);
  // The packet might not end on an even byte, but we need to make sure COWS
  // unstuffs the entire thing.
  const uint32_t cows_end = (packet_end + packet_end % 2) / 2;
  cows_->CowsUnstuff(reinterpret_cast<uint16_t *>(receive_buffer_), cows_end);
  // Don't parse the overhead word.
  const bool parsed =
      message->ParseFromArray(receive_buffer_ + 2, packet_end - 2);

  // Delete the message from the buffer.
  ClearPacket(packet_end);

  return parsed;
}

bool SimulatorCom::SyncToPacket() {
  LOG(INFO) << "Syncing to packet start.";

  const uint8_t expected[] = {0, 0};
  uint8_t separator[2];

  if (!serial_->ReceiveMessage(separator, 2)) {
    return false;
  }

  // Read words until we find the separator.
  while (memcmp(expected, separator, 2)) {
    // Shift by one so we don't miss any possible framings.
    separator[0] = separator[1];
    VLOG(1) << "Received word: " << separator[0] << " " << separator[1];

    if (!serial_->ReceiveMessage(separator + 1, 1)) {
      // Reading failure.
      return false;
    }
  }

  return true;
}

uint32_t SimulatorCom::FindPacketEnd(uint32_t start) {
  // Find the zero separator.
  for (uint32_t i = start + 1; i < receive_space_used_; ++i) {
    if (receive_buffer_[i - 1] == 0 && receive_buffer_[i] == 0) {
      // We found the separator, which marks the end of the packet.
      LOG(INFO) << "Found packed end at " << i - 1;
      return i - 1;
    }
  }

  // We didn't find the end.
  return 0;
}

void SimulatorCom::ClearPacket(uint32_t packet_end) {
  CHECK(receive_space_used_ - packet_end >= 2);
  VLOG(1) << "Clearing packet.";

  // Shift everything down, omitting the separator.
  memmove(receive_buffer_, receive_buffer_ + packet_end + 2,
          receive_space_used_ - packet_end - 2);

  // We are now clear to write after the end of whatever data is remaining.
  receive_space_used_ -= packet_end + 2;
}

}  // namespace sim
}  // namespace libmc
