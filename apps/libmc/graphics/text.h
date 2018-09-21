#ifndef LIBMC_GRAPHICS_TEXT_H_
#define LIBMC_GRAPHICS_TEXT_H_

#include "types/graphics_types.h"
#include "primitive.h"

namespace libmc {
namespace graphics {

class Text : Primitive {
 public:
  Text();
  virtual ~Text() = default;

 protected:
  virtual void Draw();
  virtual BBox GetBBox();
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_TEXT_H_
