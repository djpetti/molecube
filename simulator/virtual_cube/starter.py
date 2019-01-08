import logging
import os
import signal
import subprocess
import sys
import time

from config import config


# Configure logging.
logging.basicConfig(filename="/cube_logs/starter.log",
                    filemode="w",
                    level=logging.DEBUG)
logger = logging.getLogger(__name__)

# Load the cube configuration.
cube_config = config.cube_config()


class Starter(object):
  """ Responsible for starting all code running on the virtual cube. """

  def __init__(self):
    self.__load_config()

    # Set of running processes.
    self.__processes = set([])

    # Delete previous SHM if it's still hanging around.
    if os.path.exists(self.__shm_file):
      logger.info("Deleting existing SHM: %s" % (self.__shm_file))
      os.remove(self.__shm_file)

  def __load_config(self):
    """ Loads the configuration from a file. """
    # List of binaries to run.
    abs_binaries = cube_config.get("binaries", "start_list")

    # Convert the binary paths so they point to the copied versions in the cube
    # binary directory.
    self.__binaries = []
    bin_dir = cube_config.get("binaries", "vm_bin_dir")
    for binary in abs_binaries:
      bin_name = os.path.basename(binary)
      copied_path = os.path.join(bin_dir, bin_name)
      self.__binaries.append(copied_path)

    # Log directory for processes.
    self.__log_dir = cube_config.get("logging", "vm_log_dir")
    logger.debug("Writing process logs to '%s'." % (self.__log_dir))

    self.__shm_file = cube_config.get("shm_file")

  def __start_binary(self, binary):
    """ Starts a particular binary.
    Args:
      binary: The path to the binary. """
    logger.info("Starting binary '%s'." % (binary))

    # Specify logging dir.
    my_env = os.environ.copy()
    my_env["GLOG_log_dir"] = self.__log_dir

    # We exectute binaries in a chroot to make sure we have the libraries that
    # we need.
    command = ["/usr/sbin/chroot", "/cube_root", binary]
    process = subprocess.Popen(command, env=my_env, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, close_fds=True)
    self.__processes.add(process)

    logger.debug("Pid: %d" % (process.pid))

  def start_all(self):
    """ Starts all the binaries. """
    for binary in self.__binaries:
      self.__start_binary(binary)

  def stop_all(self):
    """ Stops all currently-running processes. """
    logger.info("Stopping all running processes...")

    # First, send SIGTERM.
    for process in self.__processes:
      process.terminate()
    # Now, wait for them to exit.
    for process in self.__processes:
      code = process.wait()
      if code != 0:
        logger.warning("Process %d exited with non-zero code %d." % \
                       (process.pid, code))

    logger.info("All processes terminated.")

  def check_running(self):
    """ Verifies that all the processes are still running.
    Args:
      Returns true if all processes are running, false otherwise. """
    all_running = True

    to_delete = []
    for process in self.__processes:
      code = process.poll()
      if code is not None:
        # Process exited.
        logger.error("Process %d exited unexpectedly with return code %d" % \
                     (process.pid, code))

        all_running = False
        to_delete.append(process)

    for process in to_delete:
      self.__processes.remove(process)

    return all_running

  def check_output(self):
    """ Reads and logs output from all processes. """
    for process in self.__processes:
      output = process.stdout.read()
      if output:
        # Process produced some output.
        logger.warning("Got output from process %d: %s" % (process.pid, output))


def main():
  def exit_handler(*args):
    """ Handler for exit signal. """
    logger.info("Caught signal, will now exit.")

    # Gracefully stop all the processes.
    starter.stop_all()

    sys.exit(0)

  # Start everything.
  starter = Starter()
  starter.start_all()

  # Register the exit handler.
  signal.signal(signal.SIGINT, exit_handler)
  signal.signal(signal.SIGTERM, exit_handler)

  # Keep tabs on the processes so we can yell if they exit.
  while True:
    # TODO (danielp): Auto-restart of failed processes?
    starter.check_running()
    starter.check_output()
    time.sleep(1)

if __name__ == "__main__":
  main()
