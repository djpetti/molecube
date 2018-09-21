#!/usr/bin/python


import my_app
import tabletop


table = tabletop.Tabletop()
table.make_cube(color=tabletop.Cube.Colors.RED)
table.make_cube(color=tabletop.Cube.Colors.BLUE)
#table.make_cube(color=tabletop.Cube.Colors.GOLD)

app = my_app.MyApp
table.start_app_on_all(app)

table.run()
