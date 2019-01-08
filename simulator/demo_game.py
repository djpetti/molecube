#!/usr/bin/python

# Import custom logger to initialize logging system. This must be done before
# anything tries to use logging.
import cube_logger

from config import config
import word_app
import tabletop


# Load the simulator config.
sim_config = config.simulator_config()


table = tabletop.Tabletop()

colors = sim_config.subkeys("appearance", "colors")
letters = ["E", "T", "A", "N", "S"]
for i, letter in enumerate(letters):
  color = colors[i % len(colors)]
  cube = table.make_cube(color=color)

  # Start the letter app.
  app = word_app.WordGameLetter(letter)
  cube.run_app(app)

# Create the checker cube.
gold_color = sim_config.get("appearance", "colors", "cube_gold")
checker_cube = table.make_cube(color=gold_color)
app = word_app.WordGameChecker()
checker_cube.run_app(app)

table.run()
