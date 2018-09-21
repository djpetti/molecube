#!/usr/bin/python


import word_app
import tabletop


table = tabletop.Tabletop()

colors = [tabletop.Cube.Colors.RED, tabletop.Cube.Colors.BLUE,
          tabletop.Cube.Colors.GOLD]
letters = ["E", "T", "A", "N", "S"]
for i, letter in enumerate(letters):
  color = colors[i % len(colors)]
  cube = table.make_cube(color=tabletop.Cube.Colors.RED)

  # Start the letter app.
  app = word_app.WordGameLetter(letter)
  cube.run_app(app)

# Create the checker cube.
checker_cube = table.make_cube(color=tabletop.Cube.Colors.GOLD)
app = word_app.WordGameChecker()
checker_cube.run_app(app)

table.run()
