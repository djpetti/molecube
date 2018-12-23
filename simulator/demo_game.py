#!/usr/bin/python

# Import custom logger to initialize logging system. This must be done before
# anything tries to use logging.
import cube_logger


import config
import word_app
import tabletop


table = tabletop.Tabletop()

colors = list(config.items('COLORS'))
letters = ["E", "T", "A", "N", "S"]
for i, letter in enumerate(letters):
  color = colors[i % len(colors)]
  cube = table.make_cube(color=config.get('COLORS', 'CUBE_RED'))

  # Start the letter app.
  app = word_app.WordGameLetter(letter)
  cube.run_app(app)

# Create the checker cube.
checker_cube = table.make_cube(color=config.get('COLORS', 'CUBE_GOLD'))
app = word_app.WordGameChecker()
checker_cube.run_app(app)

table.run()
