#include "graphics.h"

// TODO (danielp): Fill in stub methods.

namespace libmc {
namespace graphics {

Graphics *Graphics::graphics_ = nullptr;

Graphics &Graphics::GetGraphics() {
  if (!Graphics::graphics_) {
    // Create a new graphics object.
    Graphics::graphics_ = new Graphics();
  }

  // We already have an object.
  return *Graphics::graphics_;
}

Rectangle &Graphics::DrawRect(const Point &center, uint32_t width,
                              uint32_t height) {
  // Resize to hold the new rectangle.
  rectangles_.resize(rectangles_.size() + 1);
  return rectangles_.back();
}

Oval &Graphics::DrawOval(const Point &center, uint32_t width, uint32_t height) {
  // Resize to hold new oval.
  ovals_.resize(ovals_.size() + 1);
  return ovals_.back();
}

Text &Graphics::DrawText(const Point &pos, uint16_t size) {
  // Resize to hold new text.
  text_.resize(text_.size() + 1);
  return text_.back();
}

void DeleteItem(Primitive *item) {}

void SetBackgroundColor(const Color &color) {}

void Repaint() {}

}  // namespace graphics
}  // namespace libmc
