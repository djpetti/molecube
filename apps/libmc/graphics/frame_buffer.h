#ifndef LIBMC_GRAPHICS_FRAME_BUFFER_H_
#define LIBMC_GRAPHICS_FRAME_BUFFER_H_

#include <stdint.h>

namespace libmc {
namespace graphics {

// A frame buffer stores data that will be used by the graphics system. This is
// a very general class that allows storing pretty much anything.
class FrameBuffer {
 public:
  // Args:
  //  height: The height of the buffer, in pixels.
  //  width: The width of the buffer, in pixels.
  //  channels: The number of 8-bit color channels.
  FrameBuffer(uint32_t height, uint32_t width, uint32_t channels);
  ~FrameBuffer();

  // Framebuffers can't be copied, but they can be moved.
  FrameBuffer(const FrameBuffer &other) = delete;
  FrameBuffer(const FrameBuffer &&other);

  // Copy data into the buffer.
  // Args:
  //  data: The data to copy in. Should be in row-major order, with packed
  //        channels.
  void CopyIn(const uint8_t *data);
  // Copy data out of the buffer.
  // Args:
  //  dest: Location to copy data to. Data will be copied in row-major order,
  //        with packed channels.
  void CopyOut(uint8_t *data) const;

  // Sets the value of a particular subpixel.
  // Args:
  //  row_index: The row index of the subpixel.
  //  col_index: The column index of the subpixel.
  //  chan_index: The channel index of the subpixel.
  //  value: The value to set.
  void SetSubpixel(uint32_t row_index, uint32_t col_index, uint32_t chan_index,
                   uint8_t value);

  // Gets the underlying buffer. This is mostly here so we can easily get a
  // place to point DMA handlers to.
  // Returns:
  //  The underlying buffer.
  const uint8_t *GetBuffer() const;

 private:
  // The underlying memory backing the buffer.
  uint8_t *buffer_;
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_FRAME_BUFFER_H_
