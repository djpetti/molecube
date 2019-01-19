#ifndef LIBMC_CORE_EVENTS_SYSTEM_EVENT_LISTENER_H_
#define LIBMC_CORE_EVENTS_SYSTEM_EVENT_LISTENER_H_

#include <memory>

#include "tachyon/lib/queue_interface.h"

#include "event_listener_interface.h"
#include "event.h"
#include "system_event.h"

namespace libmc {
namespace core {
namespace events {

// Listener for system events.
class SystemEventListener : public EventListenerInterface {
 public:
  // Typedef for the queue type.
  typedef ::std::unique_ptr<::tachyon::QueueInterface<SystemEvent>> QueuePtr;

  // Gets a singleton instance of this class.
  // Returns:
  //  The singleton instance, creating it if necessary.
  static SystemEventListener &GetInstance();
  // Allows for injection of the queue during testing. This will always create a
  // new instance, thus breaking the singleton property. Therefore, it should
  // only be used for testing.
  // Args:
  //  queue: The new queue to use for this instance.
  // Returns:
  //  The instance that it created.
  static SystemEventListener &CreateWithQueue(const QueuePtr &queue);

  virtual ~SystemEventListener() = default;

  // Disable copy and assignment.
  SystemEventListener(SystemEventListener const &other) = delete;
  void operator=(SystemEventListener const &other) = delete;

  virtual void Listen(EventCommon *event);
  virtual bool Get(EventCommon *event);

 private:
  // Private constructor to force the use of singleton.
  // Args:
  //  queue: The queue to read events on.
  SystemEventListener(const QueuePtr &queue);

  // Queue for reading system events on.
  const QueuePtr &queue_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_SYSTEM_EVENT_LISTENER_H_
