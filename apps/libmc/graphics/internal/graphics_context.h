#ifndef MCLIB_GRAPHICS_INTERNAL_GRAPHICS_CONTEXT_H_
#define MCLIB_GRAPHICS_INTERNAL_GRAPHICS_CONTEXT_H_

#include <stdint.h>

#include <vector>

#include "apps/libmc/graphics/types/graphics_types.h"

namespace libmc {
namespace graphics {
namespace internal {

// Represents the value of a single pixel on the screen.
struct Pixel {
  // Pixel red value.
  uint8_t Red;
  // Pixel green value.
  uint8_t Green;
  // Pixel blue value.
  uint8_t Blue;

  // Special flag. If set, it indicates that we want to ignore the set values
  // and keep this pixel the same as whatever it was.
  bool NoChange;
};

// A graphics context handles drawing actual pixels on the screen. This
// functionality is broken out so that end-users don't have access to the
// low-level drawing mechanics.
// TODO (danielp): Since this class is only used internally, I'm being lazy and
//                 not specing it fully. That will need to happen eventually.
class GraphicsContext {
 public:
  // Paints a rectangular region of the screen.
  // Args:
  //  top_left: The top left corner of the region.
  //  bot_right: The bottom right corner of the region.
  //  pixels: The pixel values for the region, in row-major order.
  void PaintRegion(const Point &top_left, const Point &bot_right,
                   const ::std::vector<Pixel> &pixels);
  // Clears a region of the screen, returning it to the background color.
  // Args:
  //  top_left: The top left corner of the region.
  //  bot_right: The bottom right corner of the region.
  void ClearRegion(const Point &top_left, const Point &bot_right);
};

}  // namespace internal
}  // namespace graphics
}  // namespace libmc

#endif  // MCLIB_GRAPHICS_INTERNAL_GRAPHICS_CONTEXT_H_
