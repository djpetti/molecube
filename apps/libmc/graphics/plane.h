#ifndef LIBMC_GRAPHICS_PLANE_H_
#define LIBMC_GRAPHICS_PLANE_H_

#include <stdint.h>

#include "apps/libmc/graphics/types/graphics_types.h"
#include "frame_buffer.h"

namespace libmc {
namespace graphics {

// In DRM parlance, a plane represents an image source that can be blended with
// or overlayed onto other planes. A plane is basically nothing more than a
// frame buffer with some specific methods that make it easier to manipulate
// image data.
class Plane {
 public:
  // Args:
  //  height: The height of the plane, in pixels.
  //  width: The width of the plane, in pixels.
  Plane(uint16_t height, uint16_t width);
  virtual ~Plane();

  // Sets the content of the plane.
  // Args:
  //  data: The plane contents in packed RGB 24-bit format. Should be in
  //        row-major order.
  virtual void SetDataRgb(const uint8_t *data);
  // Sets the content of the plane, with an alpha channel.
  // Args:
  //  data: The plane contents in packed RGBA 32-bit format. Should be in
  //        row-major order.
  virtual void SetDataRgba(const uint8_t *data);

  // Sets the global alpha value for the plane.
  // Args:
  //  alpha: The 8-bit alpha value to use.
  virtual void SetGlobalAlpha(uint8_t alpha);

  // Sets a new "window" for this plane. This does not actually change anything
  // in the underlying frame buffer, it only chooses a part of the buffer that
  // will be displayed. It's a similar idea to ROIs in OpenCV.
  //
  // Once the window is set, the SetDataX method will only set data in that
  // window, and will expect appropriately-sized arrays.
  // Args:
  //  window: The window that will be set.
  virtual void SetWindow(const BBox &window);
  // Gets the currently-set "window" for this plane.
  // Returns:
  //  The window that is currently set. If no window has been explicitly set,
  //  the window returned will encompass the entire plane.
  virtual const BBox &GetWindow() const;

  // Switches the buffer that is used for scanout. This must be called after any
  // changes are made to the data in the plane in order for those changes to be
  // reflected on-screen. The actual swapping will occur on the next v-blank.
  virtual void SwapBuffers();

  // Gets the underlying buffer. This is mostly here so we can easily get a
  // place to point DMA handlers to.
  // Returns:
  //  The underlying buffer.
  const uint8_t *GetBuffer() const;

 private:
  // The underlying frame buffers for the plane. (We use double-buffering.)
  FrameBuffer buffers_[2];
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_PLANE_H_
