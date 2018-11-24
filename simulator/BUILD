py_library(
  name = "simulator",
  srcs = glob(["*.py"], exclude=["demo_game.py"]),
  data = ["config.ini"],
  deps = ["//simulator/virtual_cube"],
  visibility = ["//simulator:__subpackages__"],
)

py_binary(
  name = "demo_game",
  srcs = ["demo_game.py"],
  deps = [":simulator"],
)
