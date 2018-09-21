#ifndef LIBMC_GRAPHICS_GRAPHICS_TYPES_H_
#define LIBMC_GRAPHICS_GRAPHICS_TYPES_H_

#include <stdint.h>

namespace libmc {
namespace graphics {

// A structure defining a 2D point.
struct Point {
  uint16_t X;
  uint16_t Y;
};

// A structure for defining a color, in RGB form.
struct Color {
  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
};

// A structure for defining a bounding box.
struct BBox {
  // The top left corner of the bounding box.
  Point TopLeft;
  // The bottom right corner of the bounding box.
  Point BotRight;
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_TYPES_H_
