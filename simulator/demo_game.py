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
# Eliminate colors that are not cube colors.
colors = [color for color in colors if color.startswith("cube")]

letters = ["E"]
for i, letter in enumerate(letters):
  color_name = colors[i % len(colors)]
  color = sim_config.get("appearance", "colors", color_name)
  cube = table.make_cube(color=color)

# Create the checker cube.
gold_color = sim_config.get("appearance", "colors", "cube_gold")
checker_cube = table.make_cube(color=gold_color)

table.run()
