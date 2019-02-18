#include "canvas.h"

// TODO (danielp): Fill in stub methods.

namespace libmc {
namespace graphics {

Canvas &Canvas::GetCanvas() {
  // Create a new canvas if one doesn't exist.
  static Canvas canvas;
  return canvas;
}

Rectangle &Canvas::DrawRect(const Point &center, uint32_t width,
                              uint32_t height) {
  // Resize to hold the new rectangle.
  rectangles_.resize(rectangles_.size() + 1);
  return rectangles_.back();
}

Oval &Canvas::DrawOval(const Point &center, uint32_t width, uint32_t height) {
  // Resize to hold new oval.
  ovals_.resize(ovals_.size() + 1);
  return ovals_.back();
}

Text &Canvas::DrawText(const Point &pos, uint16_t size) {
  // Resize to hold new text.
  text_.resize(text_.size() + 1);
  return text_.back();
}

void DeleteItem(Primitive *item) {}

void SetBackgroundColor(const Color &color) {}

void Repaint() {}

}  // namespace graphics
}  // namespace libmc
