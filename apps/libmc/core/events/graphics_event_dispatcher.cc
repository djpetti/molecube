#include "graphics_event_dispatcher.h"

#include <string.h>

#include "glog/logging.h"

#include "tachyon/lib/queue.h"

#include "apps/libmc/constants.h"

namespace libmc {
namespace core {
namespace events {

using constants::kQueueNames;
using ::tachyon::Queue;

GraphicsEventDispatcher &GraphicsEventDispatcher::GetInstance() {
  VLOG(1) << "Fetching queue: " << kQueueNames.GraphicsQueue;

  // This is a slight abuse of the static keyword, but it makes our life easier
  // by guaranteeing that this will be created once and eventually destroyed.
  static const QueuePtr &queue =
      Queue<GraphicsEvent>::FetchProducerQueue(kQueueNames.GraphicsQueue);
  static GraphicsEventDispatcher instance(queue);
  return instance;
}

GraphicsEventDispatcher &GraphicsEventDispatcher::CreateWithQueue(
    const QueuePtr &queue) {
  static GraphicsEventDispatcher instance(queue);
  return instance;
}

GraphicsEventDispatcher::GraphicsEventDispatcher(const QueuePtr &queue)
    : queue_(queue), event_(CHECK_NOTNULL(new GraphicsEvent)) {
  // Permanently set the type of the event buffer.
  event_->Common.Type = EventType::GRAPHICS;
}

GraphicsEventDispatcher::~GraphicsEventDispatcher() {
  // Free the event buffer.
  delete event_;
}

bool GraphicsEventDispatcher::Dispatch(EventCommon *event) {
  DLOG_IF(FATAL, !event) << "event cannot be NULL.";

  // First, cast to the proper event type.
  GraphicsEvent *graphics_event = reinterpret_cast<GraphicsEvent *>(event);

  // Set the header fields.
  graphics_event->Common.Type = EventType::GRAPHICS;

  // Send the message.
  if (!queue_->EnqueueBlocking(*graphics_event)) {
    // This really shouldn't fail unless we don't have a consumer, which would
    // imply that the simulator process died.
    LOG(ERROR) << "Enqueue failed. Is simulator_process running?";
    return false;
  }

  return true;
}

bool GraphicsEventDispatcher::Dispatch(const uint8_t *image_data) {
  DLOG_IF(FATAL, !image_data) << "image_data cannot be NULL.";

  // Copy the data into the event buffer.
  memcpy(event_->Image, image_data,
         kSimulator.ScreenWidth * kSimulator.ScreenHeight * 3);

  // Dispatch the event.
  return Dispatch(&(event_->Common));
}

}  // namespace events
}  // namespace core
}  // namespace libmc
