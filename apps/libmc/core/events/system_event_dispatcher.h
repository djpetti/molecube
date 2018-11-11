#ifndef LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_
#define LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_

#include "google/protobuf/message_lite.h"

#include "apps/libmc/core/system_manager_interface.h"
#include "event.h"
#include "event_dispatcher_interface.h"

namespace libmc {
namespace core {
namespace events {

// System event. This event handles commands to the base system.
struct SystemEvent {
  EventCommon Common;

  // Whether we want to shutdown the system.
  bool Shutdown;
};

// Dispatcher for System events.
class SystemEventDispatcher : public EventDispatcherInterface {
 public:
  // Gets a singleton instance of this class.
  // Returns:
  //  The singleton instance, creating it if necessary.
  static SystemEventDispatcher &GetInstance();

  // Allows for dependency injection during testing.
  // Args:
  //  manager: The SystemManager to use.
  SystemEventDispatcher(SystemManagerInterface *manager);
  virtual ~SystemEventDispatcher();

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
  SystemEventDispatcher();

  // SystemManager instance that we use to execute commands.
  SystemManagerInterface *manager_;
  // Whether we own the manager object.
  bool own_manager_ = false;
};

}  // namespace events
}  // namespace core
}  // namespace libmc

#endif  // LIBMC_CORE_EVENTS_SYSTEM_EVENT_DISPATCHER_H_
