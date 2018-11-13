#ifndef LIBMC_CORE_NEIGHBORHOOD_H_
#define LIBMC_CORE_NEIGHBORHOOD_H_

#include <stdint.h>

namespace libmc {
namespace core {

// Represents the connection configuration of a cube.
struct Connections {
  // Whether a cube is connected to the top face of this one.
  bool Top;
  // Whether a cube is connected to the bottom face of this one.
  bool Bottom;
  // Whether a cube is connected to the left face of this one.
  bool Left;
  // Whether a cube is connected to the right face of this one.
  bool Right;
};

// Enumerates the faces of a cube.
enum class Face { TOP, BOTTOM, LEFT, RIGHT };

// An abstract base class for dealing with connected cubes.
class Neighborhood {
 public:
  virtual ~Neighborhood() = default;

  // Gets the current connection configuration of the cube.
  // Returns:
  //  A Connections object representing the current configuration.
  const Connections &GetConnections();

  // Sends a message to a particular cube.
  // Args:
  //  face: The face on this cube that the recipient is connected to.
  //  message: The message to send. Assumed to have a NULL terminator.
  // Returns:
  //  True if the message was sent successfully, false if no cube is connected.
  bool SendMessage(Face face, const char *message);
  // Sends a binary message to a particular cube.
  // Args:
  //  face: The face on this cube that the recipient is connected to.
  //  message: The message to send.
  //  length: The length of the message in bytes.
  // Returns:
  //  True if the message was sent successfully, false if no cube is connected.
  bool SendBinaryMessage(Face face, const void *message, uint32_t length);

  // Waits indefinitely for an event. An event is either the connection
  // configuration being changed, or a message being received. Will not return
  // until an event occurs and the appropriate handler is run.
  void WaitForEvent();
  // Checks if an event has occurred. If so, it runs the appropriate handler. It
  // will return immediately, regardless of whether an event occurs or not.
  void PollForEvent();

 protected:
  // This method is called when the cube's connection configuration changes.
  virtual void OnConfigChanged() = 0;

  // This method is called when a new message is received.
  // Args:
  //  face: The face that the sending cube is connected to.
  //  message: The message that was received.
  virtual void OnMessageReceived(Face face, const char *message) = 0;
  // This method is called when a new binary message is received.
  // Args:
  //  face: The face that the sending cube is connected to.
  //  message: The message that was received.
  //  length: The length of the message, in bytes.
  virtual void OnBinaryMessageReceived(Face face, const void *message,
                                       uint32_t length) = 0;

 private:
  // Indicates which cubes are connected to which faces at this time.
  Connections connections_;
};

}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_NEIGHBORHOOD_H_
