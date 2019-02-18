#ifndef LIBMC_GRAPHICS_GRAPHICS_H_
#define LIBMC_GRAPHICS_GRAPHICS_H_

#include "plane.h"

namespace libmc {
namespace graphics {

// This is the absolute lowest-level graphics API that users have access to,
// and it is designed as a thin wrapper around the kernel DRM system. Graphics
// is the main class, which manages the entire thing.
class Graphics {
 public:
  // Planes that are supported by the graphics hardware.
  enum class SystemPlane {
    HEO,
    OVR1,
    OVR2,
    BASE
  };

  // Gets the singleton graphics instance for the system, creating it if it
  // doesn't exist.
  static Graphics &GetInstance();

  // Gets a particular plane, which can then be manipulated directly.
  // Args:
  //  plane: The plane to get.
  // Returns:
  //  The selected plane.
  Plane &GetPlane(SystemPlane plane) const;

  // Convenience method that swaps the buffers for all planes.
  void SwapBuffers();

 private:
  // Private constructor to force use of singleton.
  Graphics();
};

}  // namespace graphics
}  // namespace libmc

#endif  // LIBMC_GRAPHICS_GRAPHICS_H_
