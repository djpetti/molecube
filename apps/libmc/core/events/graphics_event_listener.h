#ifndef LIBMC_CORE_EVENTS_GRAPHICS_EVENT_LISTENER_H_
#define LIBMC_CORE_EVENTS_GRAPHICS_EVENT_LISTENER_H_

#include <memory>

#include "google/protobuf/message_lite.h"

#include "tachyon/lib/queue_interface.h"

#include "event.h"
#include "graphics_event.h"
#include "proto_event_listener_interface.h"

namespace libmc {
namespace core {
namespace events {

// Listener for graphics events.
class GraphicsEventListener : public ProtoEventListenerInterface {
 public:
  // Typedef for the queue type.
  typedef ::std::unique_ptr<::tachyon::QueueInterface<GraphicsEvent>> QueuePtr;

  // Gets a singleton instance of this class.
  // Returns:
  //  The singleton instance, creating it if necessary.
  static GraphicsEventListener &GetInstance();
  // Allows for injection of the queue during testing. This will always create a
  // new instance, thus breaking the singleton property. Therefore, it should
  // only be used for testing.
  // Args:
  //  queue: The new queue to use for this instance.
  // Returns:
  //  The instance that it created.
  static ::std::unique_ptr<GraphicsEventListener> CreateWithQueue(
      const QueuePtr &queue);

  virtual ~GraphicsEventListener();

  // Disable copy and assignment.
  GraphicsEventListener(GraphicsEventListener const &other) = delete;
  void operator=(GraphicsEventListener const &other) = delete;

  virtual void Listen(EventCommon *event);
  virtual bool Get(EventCommon *event);

  virtual void ListenProtobuf(::google::protobuf::MessageLite *message);
  virtual bool GetProtobuf(::google::protobuf::MessageLite *message);

 private:
  // Private constructor to force the use of singleton.
  // Args:
  //  queue: The queue to read events on.
  GraphicsEventListener(const QueuePtr &queue);

  // Queue for reading graphics events on.
  const QueuePtr &queue_;

  // Buffer we use for events that are received internally.
  GraphicsEvent *event_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_GRAPHICS_EVENT_LISTENER_H_
