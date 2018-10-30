py_library(
  name = "simulator",
  srcs = glob(["*.py"], exclude=["demo_game.py"]),
)

py_binary(
  name = "demo_game",
  srcs = ["demo_game.py"],
  deps = [":simulator"],
)
