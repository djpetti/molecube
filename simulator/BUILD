py_library(
  name = "simulator",
  srcs = glob(["*.py"], exclude=["demo_game.py"]),
  data = ["config.ini"],
  deps = ["//simulator/virtual_cube",
          "//config",
          "//apps/libmc/sim/protobuf:python_graphics_message"],
  visibility = ["//simulator:__subpackages__"],
)

py_binary(
  name = "demo_game",
  srcs = ["demo_game.py"],
  deps = [":simulator"],
)
