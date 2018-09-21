#include "neighborhood.h"

// TODO (danielp): Fill in stubs.

namespace libmc {

const Connections &Neighborhood::GetConnections() { return connections_; }

bool Neighborhood::SendMessage(Face face, const char *message) {
  return false;
}

bool Neighborhood::SendBinaryMessage(Face face, const void *message,
                                     uint32_t length) {
  return false;
}

void Neighborhood::WaitForEvent() {}

void PollForEvent() {}

}  // namespace libmc
