#include "graphics_event_listener.h"

#include <stdint.h>

#include "glog/logging.h"

#include "tachyon/lib/queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/sim/protobuf/graphics_message.pb.h"

namespace libmc {
namespace core {
namespace events {
namespace {

using constants::kQueueNames;
using constants::kSimulator;
using ::google::protobuf::MessageLite;
using sim::GraphicsMessage;
using ::tachyon::Queue;

// Number of pixels in the image.
constexpr uint32_t kNumPixels =
    kSimulator.ScreenWidth * kSimulator.ScreenHeight * 3;

// Populates a protobuf message based on the contents of an event.
// Args:
//  event: The event.
//  message: The message to populate.
void EventToMessage(const GraphicsEvent *event, GraphicsMessage *message) {
  message->set_op_type(GraphicsMessage::PAINT);
  message->mutable_image()->set_width(kSimulator.ScreenWidth);
  message->mutable_image()->set_height(kSimulator.ScreenHeight);
  message->mutable_image()->set_data(event->Image, kNumPixels);
}

}  // namespace

GraphicsEventListener &GraphicsEventListener::GetInstance() {
  VLOG(1) << "Fetching queue: " << kQueueNames.GraphicsQueue;

  // This is a slight abuse of the static keyword, but it makes our life easier
  // by guaranteeing that this will be created once and eventually destroyed. We
  // fetch a queue of size 1 because the frame data is large, and we don't want
  // to have to store any more of it than necessary in SHM.
  static const QueuePtr &queue =
      Queue<GraphicsEvent>::FetchSizedQueue(kQueueNames.GraphicsQueue, 1);
  static GraphicsEventListener instance(queue);
  return instance;
}

::std::unique_ptr<GraphicsEventListener> GraphicsEventListener::CreateWithQueue(
    const QueuePtr &queue) {
  GraphicsEventListener *instance = new GraphicsEventListener(queue);
  return ::std::unique_ptr<GraphicsEventListener>(instance);
}

GraphicsEventListener::GraphicsEventListener(const QueuePtr &queue)
    : queue_(queue), event_(CHECK_NOTNULL(new GraphicsEvent)) {}

GraphicsEventListener::~GraphicsEventListener() {
  delete event_;
}

void GraphicsEventListener::Listen(EventCommon *event) {
  DLOG_IF(FATAL, !event) << "'event' should not be NULL.";
  // Assume the event is of the correct type.
  GraphicsEvent *graphics_event = reinterpret_cast<GraphicsEvent *>(event);

  // Wait for the next event.
  queue_->DequeueNextBlocking(graphics_event);
  VLOG(1) << "Received new event.";
  DLOG_IF(FATAL, graphics_event->Common.Type != EventType::GRAPHICS)
      << "Did not get graphics event?";
}

bool GraphicsEventListener::Get(EventCommon *event) {
  DLOG_IF(FATAL, !event) << "'event' should not be NULL.";
  // Assume the event is of the correct type.
  GraphicsEvent *graphics_event = reinterpret_cast<GraphicsEvent *>(event);

  // Try getting the next event.
  if (!queue_->DequeueNext(graphics_event)) {
    // No available events.
    return false;
  }

  VLOG(1) << "Received new event.";
  DLOG_IF(FATAL, graphics_event->Common.Type != EventType::GRAPHICS)
      << "Did not get graphics event?";

  return true;
}

void GraphicsEventListener::ListenProtobuf(MessageLite *message) {
  DLOG_IF(FATAL, !message) << "'message' should not be NULL.";
  // Downcast to the expected message type.
  GraphicsMessage *graphics_message = dynamic_cast<GraphicsMessage *>(message);
  DLOG_IF(FATAL, !graphics_message)
      << "'messsage' must be of GraphicsMessage type";

  // First, listen for the event.
  Listen(&(event_->Common));

  // Convert the event to a protobuf message._
  EventToMessage(event_, graphics_message);
}

bool GraphicsEventListener::GetProtobuf(MessageLite *message) {
  DLOG_IF(FATAL, !message) << "'message' should not be NULL.";
  // Downcast to the expected message type.
  GraphicsMessage *graphics_message = dynamic_cast<GraphicsMessage *>(message);
  DLOG_IF(FATAL, !graphics_message)
      << "'messsage' must be of GraphicsMessage type";

  // First, get the event.
  if (!Get(&(event_->Common))) {
    // No events available.
    return false;
  }

  // Convert the event to a protobuf message.
  EventToMessage(event_, graphics_message);

  return true;
}

}  // namespace events
}  // namespace core
}  // namespace libmc
