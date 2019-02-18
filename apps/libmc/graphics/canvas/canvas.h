#ifndef LIBMC_GRAPHICS_CANVAS_H_
#define LIBMC_GRAPHICS_CANVAS_H_

#include <stdint.h>

#include <vector>

#include "apps/libmc/graphics/types/graphics_types.h"
#include "oval.h"
#include "primitive.h"
#include "rectangle.h"
#include "text.h"

namespace libmc {
namespace graphics {

// A high-level graphics class. Provides API for displaying things on the
// screen.
class Canvas {
 public:
  // Gets the singleton canvas instance for this application.
  // Returns:
  //  The Graphics instance.
  static Canvas &GetCanvas();

  // Draws a rectangle on the screen.
  // Args:
  //  center: The center point of the rectangle.
  //  width: The width of the rectangle, in px.
  //  height: The height of the rectangle, in px.
  // Returns:
  //  The rectangle object that we have created.
  Rectangle &DrawRect(const Point &center, uint32_t width, uint32_t height);
  // Draws an oval on the screen.
  // Args:
  //  center: The center point of the oval.
  //  width: The width of the oval, in px.
  //  height: The height of the oval, in px.
  // Returns:
  //  The oval object that we have created.
  Oval &DrawOval(const Point &center, uint32_t width, uint32_t height);
  // Draws text on the screen.
  // Args:
  //  pos: The center point of the text's position.
  //  size: The height of the text, in px.
  // Returns:
  //  The text object that we have created.
  Text &DrawText(const Point &pos, uint16_t size);

  // Deletes an item.
  // Args:
  //  item: The item to delete.
  void DeleteItem(Primitive *item);

  // Sets the screen's background color.
  // Args:
  //  color: The color to set it to.
  void SetBackgroundColor(const Color &color);

  // Forces a repaint of the screen.
  void Repaint();

 private:
  // Arrays containing every primitive in existence. We essentially use these as
  // pools to avoid allocating and deallocating memory for every primitive that
  // comes along.
  ::std::vector<Rectangle> rectangles_;
  ::std::vector<Oval> ovals_;
  ::std::vector<Text> text_;
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_CANVAS_H_
