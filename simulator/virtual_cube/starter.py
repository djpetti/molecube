import logging
import signal
import subprocess
import sys
import time

import yaml
try:
  # Import accelerated versions if we have them.
  from yaml import CLoader as Loader
except ImportError:
  from yaml import Loader


# Configure logging.
logging.basicConfig(filename="/cube_logs/starter.log",
                    filemode="w",
                    level=logging.DEBUG)
logger = logging.getLogger(__name__)


class Starter(object):
  """ Responsible for starting all code running on the virtual cube. """

  def __init__(self, config):
    """
    Args:
      config: The configuration file to use. """
    self.__load_config(config)

    # Set of running processes.
    self.__processes = set([])

  def __load_config(self, config):
    """ Loads the configuration from a file.
    Args:
      config: The file to load from. """
    logger.info("Reading configuration from file: %s" % (config))

    config_file = file(config)
    config_data = yaml.load(config_file, Loader=Loader)

    # List of binaries to run.
    self.__binaries = config_data["binaries"]

  def __start_binary(self, binary):
    """ Starts a particular binary.
    Args:
      binary: The path to the binary. """
    logger.info("Starting binary '%s'." % (binary))

    # We exectute binaries in a chroot to make sure we have the libraries that
    # we need.
    command = ["/usr/sbin/chroot", "/cube_root", binary]
    process = subprocess.Popen(command)
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
        logger.error("Process %d exited with non-zero code %d." % \
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


def main():
  def exit_handler(*args):
    """ Handler for exit signal. """
    logger.info("Caught signal, will now exit.")

    # Gracefully stop all the processes.
    starter.stop_all()

    sys.exit(0)

  # Note: We don't use argparse here because OpenWRT doesn't really support it.
  if len(sys.argv) != 2:
    print "Usage: starter.py starter_config"
    sys.exit(1)

  # Start everything.
  starter = Starter(sys.argv[1])
  starter.start_all()

  # Register the exit handler.
  signal.signal(signal.SIGINT, exit_handler)
  signal.signal(signal.SIGTERM, exit_handler)

  # Keep tabs on the processes so we can yell if they exit.
  while True:
    # TODO (danielp): Auto-restart of failed processes?
    starter.check_running()
    time.sleep(1)

if __name__ == "__main__":
  main()
