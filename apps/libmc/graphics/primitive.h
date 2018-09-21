#ifndef LIBMC_GRAPHICS_PRIMITIVE_H_
#define LIBMC_GRAPHICS_PRIMITIVE_H_

#include <stdint.h>

#include "internal/graphics_context.h"
#include "types/graphics_types.h"

namespace libmc {
namespace graphics {

// Superclass for graphics primitives.
class Primitive {
 public:
  virtual ~Primitive() = default;

  // Associates a graphics context with this primitive. This must be called
  // before anything else. The only reason it is not a constructor is so that
  // we're free to use primitives in vectors.
  void AssociateGraphicsContext(internal::GraphicsContext *context);

  // Moves the object by a certain amount.
  // Args:
  //  x_amt: Pixels to move in the x direction.
  //  y_amt: Pixels to move in the y direction.
  void Move(int32_t x_amt, int32_t y_amt);
  // Sets the position of the object directly.
  // Args:
  //  pos: The new position of the object.
  void SetPos(const Point &pos);
  // Gets the current position of the object.
  // Returns:
  //  The current position of the object.
  const Point &GetPos();

  // Sets the visibility on an object.
  // Args:
  //  visibility: If true, object will be made visible. If false, it will be
  //              made invisible.
  void SetVisibility(bool visibility);
  // Gets the current visibility of an object.
  // Returns:
  //  True if the object is visible, false otherwise.
  bool GetVisibility();

  // Sets the color of the object.
  // Args:
  //  color: The color to set.
  void SetColor(const Color &color);
  // Gets the current color of the object.
  // Returns:
  //  The current color.
  const Color &GetColor();

  // Checks whether two objects are currently overlapping.
  // Args:
  //  obj1: The first object to check.
  //  obj2: The second object to check.
  // Returns:
  //  True if the objects are overlapping, false otherwise.
  static bool CheckOverlap(const Primitive *obj1, const Primitive *obj2);

 protected:
  // Internal graphics context used for drawing.
  internal::GraphicsContext *context_;

  // Position of the primitive.
  Point pos_;
  // Visibility status of the primitive.
  bool visible_;
  // Color of the primitive.
  Color color_;

  // Draws the object on the screen. This is to be implemented by subclasses.
  virtual void Draw() = 0;
  // Gets a bounding box around the object. This is used for collision
  // detection, and should be implemented by subclasses.
  // Returns:
  //  The bounding box around the object.
  virtual BBox GetBBox() = 0;
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_PRIMITIVE_H_
