import logging
import ConfigParser 
from config import *


class MyLogger(logging.Logger):

	#get the file we want to log to
	config = ConfigParser.ConfigParser()
	config.read("config.ini")	

	def __init__(self):
        # Create the Logger
		self.log = logging.getLogger()
		self.log.setLevel(logging.DEBUG)		


		log_location = MyLogger.config.get('LOGGING', 'log_location')
		#clear our log
		open(log_location, 'w').close()
 
		# Create the Handler for logging data to a file
		logger_handler = logging.FileHandler(log_location)
		logger_handler.setLevel(logging.INFO)

		# Create a Formatter for formatting the log messages
		logger_formatter = logging.Formatter('%(name)s - %(levelname)s - %(message)s')

		# Add the Formatter to the Handler
		logger_handler.setFormatter(logger_formatter)

		# Add the Handler to the Logger
		self.log.addHandler(logger_handler)


logging.setLoggerClass(MyLogger)