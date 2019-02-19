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
  // We make the queue of size 1 because the frames take up a lot of space, and
  // we don't want to have to store any more than necessary in SHM.
  static const QueuePtr &queue =
      Queue<GraphicsEvent>::FetchSizedProducerQueue(kQueueNames.GraphicsQueue, 1);
  static GraphicsEventDispatcher instance(queue);
  return instance;
}

::std::unique_ptr<GraphicsEventDispatcher> GraphicsEventDispatcher::CreateWithQueue(
    const QueuePtr &queue) {
  GraphicsEventDispatcher *instance = new GraphicsEventDispatcher(queue);
  return ::std::unique_ptr<GraphicsEventDispatcher>(instance);
}

// Since this constructor is only used for testing, delegate to the
// EventDispatcher() constructor to disable multiplexing completely.
GraphicsEventDispatcher::GraphicsEventDispatcher(const QueuePtr &queue)
    : EventDispatcher(false),
      queue_(queue),
      event_(CHECK_NOTNULL(new GraphicsEvent)) {
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

  // We don't have a really good way to recover from an error state here, so if
  // PrepareMultiplexed() fails, it's a fatal error.
  CHECK(PrepareMultiplexed(graphics_event->Common.Type))
      << "PrepareMultiplexed() failed unexpectedly.";

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
