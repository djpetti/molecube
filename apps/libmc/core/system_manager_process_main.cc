#include "gflags/gflags.h"
#include "glog/logging.h"

#include "events/system_event_listener.h"
#include "system_manager_process.h"

using ::libmc::core::events::SystemEventListener;

int main(int argc, char **argv) {
  // Initialize logging.
  google::InitGoogleLogging(argv[0]);
  // Parse command-line flags.
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Starting system manager process.";

  // Create the listener.
  SystemEventListener &event_listener = SystemEventListener::GetInstance();

  // Create the process and run forever.
  ::libmc::core::SystemManagerProcess process(&event_listener);
  process.Run();

  LOG(ERROR) << "Exiting system manager process.";
  return 1;
}
