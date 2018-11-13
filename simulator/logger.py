import logging

class Logger(object):
	def __init__(self):
        # Create the Logger
		self.logger = logging.getLogger("cube")
		self.logger.setLevel(logging.DEBUG)

		#clear our log
		open('logging.log', 'w').close()
 
		# Create the Handler for logging data to a file
		logger_handler = logging.FileHandler('logging.log')
		logger_handler.setLevel(logging.INFO)

		# Create a Formatter for formatting the log messages
		logger_formatter = logging.Formatter('%(name)s - %(levelname)s - %(message)s')

		# Add the Formatter to the Handler
		logger_handler.setFormatter(logger_formatter)

		# Add the Handler to the Logger
		self.logger.addHandler(logger_handler)
