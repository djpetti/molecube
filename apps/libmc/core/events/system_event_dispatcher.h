#ifndef LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_

#include <memory>

#include "google/protobuf/message_lite.h"

#include "tachyon/lib/queue_interface.h"

#include "event.h"
#include "proto_event_dispatcher_interface.h"
#include "system_event.h"

namespace libmc {
namespace core {
namespace events {

// Dispatcher for System events.
class SystemEventDispatcher : public ProtoEventDispatcherInterface {
 public:
  // Typedef for the queue type.
  typedef ::std::unique_ptr<::tachyon::QueueInterface<SystemEvent>> QueuePtr;

  // Gets a singleton instance of this class.
  // Returns:
  //  The singleton instance, creating it if necessary.
  static SystemEventDispatcher &GetInstance();
  // Allows for injection of the queue during testing. This will always create a
  // new instance, thus breaking the singleton property. Therefore, it should
  // only be used for testing.
  // Args:
  //  queue: The new queue to use for this instance.
  // Returns:
  //  The instance that it created.
  static ::std::unique_ptr<SystemEventDispatcher> CreateWithQueue(
      const QueuePtr &queue);

  virtual ~SystemEventDispatcher() = default;

  // Disable copy and assignment.
  SystemEventDispatcher(SystemEventDispatcher const &other) = delete;
  void operator=(SystemEventDispatcher const &other) = delete;

  // Note that this version of Dispatch() assumes that EventCommon is part of a
  // valid SystemEvent structure.
  virtual bool Dispatch(EventCommon *event);
  // Dispatch overload that builds the event in-place.
  // Args:
  //  shutdown: Whether we want to shutdown the system.
  bool Dispatch(bool shutdown);
  // Note that this version of DispatchMessage() assumes that message is of type
  // SystemMessage.
  virtual bool DispatchMessage(const ::google::protobuf::MessageLite *message);

 private:
  // Private constructor to force use of singleton.
  // Args:
  //  queue: The queue to send events to the system manager process on.
  SystemEventDispatcher(const QueuePtr &queue);

  // Queue for communicating with the system manager process.
  const QueuePtr &queue_;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_
