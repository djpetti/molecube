#ifndef LIBMC_CORE_EVENTS_GRAPHICS_EVENT_H_
#define LIBMC_CORE_EVENTS_GRAPHICS_EVENT_H_

#include <stdint.h>

#include "apps/libmc/constants.h"
#include "event.h"

namespace libmc {
namespace core {
namespace events {

using constants::kSimulator;

// Graphics event. This event handles graphics updates.
struct GraphicsEvent {
  EventCommon Common;

  // Image data represented in pure bytes. Every 3 bytes in an RGB triple, where
  // each sub-pixel value ranges from 0-255. The image is stored in row-major
  // order. The array is allocated statically based on the specified screen
  // resolution supported by the simulator.
  uint8_t Image[kSimulator.ScreenWidth * kSimulator.ScreenHeight * 3];
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_GRAPHICS_EVENT_H_
