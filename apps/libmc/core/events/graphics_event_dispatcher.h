#ifndef LIBMC_CORE_EVENTS_GRAPHICS_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_GRAPHICS_EVENT_DISPATCHER_H_

#include <stdint.h>

#include <memory>

#include "tachyon/lib/queue_interface.h"

#include "event.h"
#include "event_dispatcher.h"
#include "graphics_event.h"

namespace libmc {
namespace core {
namespace events {

// Dispatcher for graphics events. Note that this is meant to be used with a
// multiplexer.
class GraphicsEventDispatcher : public EventDispatcher {
 public:
  // Typedef for the queue type.
  typedef ::std::unique_ptr<::tachyon::QueueInterface<GraphicsEvent>> QueuePtr;

  // Gets a singleton instance of this class.
  // Returns:
  //  The singleton instance, creating it if necessary.
  static GraphicsEventDispatcher &GetInstance();
  // Allows for injection of the queue during testing. This will always create a
  // new instance, thus breaking the singleton property. Therefore, it should
  // only be used for testing. When this is used, it also automatically disables
  // multiplexing, regardless of the multiplexing settting for this event type.
  // Args:
  //  queue: The new queue to use for this instance.
  // Returns:
  //  The instance that it created.
  static ::std::unique_ptr<GraphicsEventDispatcher> CreateWithQueue(
      const QueuePtr &queue);

  virtual ~GraphicsEventDispatcher();

  // Disable copy and assignment.
  GraphicsEventDispatcher(GraphicsEventDispatcher const &other) = delete;
  void operator=(GraphicsEventDispatcher const &other) = delete;

  // Note that this version of Dispatch() assumes that EventCommon is part of a
  // valid SystemEvent structure.
  virtual bool Dispatch(EventCommon *event);
  // Dispatch overload that builds the event in-place.
  // Args:
  //  image: The image data to use. See GraphicsEvent for the specification.
  bool Dispatch(const uint8_t *image_data);

 private:
  // Private constructor to force use of singleton.
  // Args:
  //  queue: The queue to send events to the system manager process on.
  GraphicsEventDispatcher(const QueuePtr &queue);

  // Queue for communicating with the system manager process.
  const QueuePtr &queue_;

  // This is a buffer that we use for storing events that are being dispatched.
  GraphicsEvent *event_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_GRAPHICS_EVENT_DISPATCHER_H_
