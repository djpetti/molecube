#ifndef LIBMC_GRAPHICS_RECTANGLE_H_
#define LIBMC_GRAPHICS_RECTANGLE_H_

#include "apps/libmc/graphics/types/graphics_types.h"
#include "primitive.h"

namespace libmc {
namespace graphics {

class Rectangle : Primitive {
 public:
  virtual ~Rectangle() = default;

 protected:
  virtual void Draw();
  virtual BBox GetBBox();
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_RECTANGLE_H_
