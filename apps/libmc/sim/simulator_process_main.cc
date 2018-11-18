#include "simulator_process.h"

int main() {
  // Create simulator process and run forever.
  ::libmc::sim::SimulatorProcess process;
  if (!process.Run()) {
    return 1;
  }

  return 0;
}
