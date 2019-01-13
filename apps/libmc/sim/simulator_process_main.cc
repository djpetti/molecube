#include "gflags/gflags.h"
#include "glog/logging.h"

#include "simulator_process.h"

int main(int argc, char **argv) {
  // Initialize logging.
  google::InitGoogleLogging(argv[0]);
  // Parse command-line flags.
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG(INFO) << "Starting simulator process.";

  // Create simulator process and run forever.
  ::libmc::sim::SimulatorProcess process;
  process.Run();

  LOG(ERROR) << "Exiting simulator process.";
  return 0;
}
