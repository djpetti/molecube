#include "gflags/gflags.h"
#include "glog/logging.h"

#include "system_manager_process.h"

int main(int argc, char **argv) {
  // Initialize logging.
  google::InitGoogleLogging(argv[0]);
  // Parse command-line flags.
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Starting system manager process.";

  // Create the process and run forever.
  ::libmc::core::SystemManagerProcess process;
  process.Run();

  LOG(ERROR) << "Exiting system manager process.";
  return 1;
}
