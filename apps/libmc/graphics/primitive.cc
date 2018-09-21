#include "primitive.h"

namespace libmc {
namespace graphics {

// TODO (danielp): Fill in stub methods.

void Primitive::AssociateGraphicsContext(internal::GraphicsContext *context) {}

void Primitive::Move(int32_t x_amt, int32_t y_amt) {}

void Primitive::SetPos(const Point &pos) {}

const Point &Primitive::GetPos() { return pos_; }

void Primitive::SetVisibility(bool visibility) {}

bool Primitive::GetVisibility() { return visible_; }

void Primitive::SetColor(const Color &color) {}

const Color &Primitive::GetColor() { return color_; }

bool Primitive::CheckOverlap(const Primitive *obj1, const Primitive *obj2) {
  return false;
}

}  // namespace graphics
}  // namespace libmc
