py_library(
  name = "simulator",
  srcs = glob(["*.py"], exclude=["demo_game.py"]),
  data = ["config.ini"],
)

py_binary(
  name = "demo_game",
  srcs = ["demo_game.py"],
  deps = [":simulator"],
)
