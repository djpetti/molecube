#include "gflags/gflags.h"
#include "glog/logging.h"

#include "tachyon/lib/queue.h"

#include "apps/libmc/constants.h"
#include "apps/libmc/core/events/system_event.h"
#include "system_manager_process.h"

using ::libmc::constants::kQueueNames;
using ::libmc::core::events::SystemEvent;

int main(int argc, char **argv) {
  // Initialize logging.
  google::InitGoogleLogging(argv[0]);
  // Parse command-line flags.
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Starting system manager process.";

  // Fetch the queue to use.
  VLOG(1) << "Fetching queue: " << kQueueNames.SysManagerQueue;
  auto queue =
      ::tachyon::Queue<SystemEvent>::FetchQueue(kQueueNames.SysManagerQueue);

  // Create the process and run forever.
  ::libmc::core::SystemManagerProcess process(queue);
  while (process.RunIteration())
    ;

  LOG(ERROR) << "Exiting system manager process.";
  return 1;
}
