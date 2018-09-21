#ifndef LIBMC_GRAPHICS_OVAL_H_
#define LIBMC_GRAPHICS_OVAL_H_

#include "types/graphics_types.h"
#include "primitive.h"

namespace libmc {
namespace graphics {

class Oval : Primitive {
 public:
  Oval();
  virtual ~Oval() = default;

 protected:
  virtual void Draw();
  virtual BBox GetBBox();
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_OVAL_H_
