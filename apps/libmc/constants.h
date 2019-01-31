#ifndef LIBMC_CONSTANTS_H_
#define LIBMC_CONSTANTS_H_

#include <stdint.h>

namespace libmc {
namespace constants {

struct Simulator {
  // The device to use for serial communication with the simulator host.
  const char *Device;
  // The baud rate to use for communication.
  uint32_t BaudRate;

  // The maximum packet size to allow. Increasing this allows for more
  // flexibility but increases the memory burden.
  uint32_t MaxPacketSize;

  // The width of the simulated device screen, in pixels.
  uint16_t ScreenWidth;
  // The height of the simulated device screen, in pixels.
  uint16_t ScreenHeight;
};

// Names to use for queues.
struct QueueNames {
  const char *SysManagerQueue;
  const char *GraphicsQueue;
  const char *EventNotifyQueue;
};

constexpr Simulator kSimulator = {"/dev/vport1p1", 115200, 1024, 160, 128};
constexpr QueueNames kQueueNames = {"SysManagerQueue", "GraphicsQueue",
                                    "EventNotifyQueue"};

// The buffer size should be even for COWS to work reliably.
static_assert(kSimulator.MaxPacketSize % 2 == 0);

}  // namespace constants
}  // namespace libmc

#endif  // LIBMC_CONSTANTS_H_
