import os
import unittest

os.path.join(os.path.dir(__file__), "simulator")

import cube
import tabletop
import word_app
import my_app

class TestCube(unittest.TestCase):

	def setUp(self):
		table = tabletop.Tabletop()
		cube = table.make_cube(color=tabletop.Cube.Colors.RED)
	
	def test_sides(self):
		a = 1
		#self.assertEqual(cube.LEFT, "left")

class TestTabletop(unittest.TestCase):

	def setUp(self):
		table = tabletop.Tabletop()

    def test_make_cube(self):
    	

	def test_start_app_on_all(self):
		table = tabletop.Tabletop()
		cube1 = table.make_cube(color=tabletop.Cube.Colors.RED)
		cube2 = table.make_cube(color=tabletop.Cube.Colors.RED)
		app1 = word_app.WordGameLetter("A")
		app2 = word_app.WordGameLetter("B")

		cube1.run_app(app1)
		cube2.run_app(app2)

		self.assertEqual(cube1.get_app().get_letter(), app1.get_letter())
		self.assertEqual(cube2.get_app().get_letter(), app2.get_letter())
		self.assertFalse(app1.get_letter() == app2.get_letter())

		table.start_app_on_all(app1)

		self.assertEqual(cube1.get_app().get_letter(), app1.get_letter())
		self.assertEqual(cube1.get_app().get_letter(), cube2.get_app().get_letter())












if __name__ == '__main__':
    unittest.main()