py_library(
  name = "virtual_cube",
  srcs = glob(["*.py"]),
  data = glob(["assets/*"]),
  deps = ["//apps/libmc/sim/protobuf:python_sim_message"],
  visibility = ["//simulator/virtual_cube/tests:__pkg__"],
)

py_library(
  # TODO (danielp) This target is a really hacky workaround for a bug in the
  # Python protobuf rules that causes it to add duplicate entries to sys.path.
  # Change this as soon as it gets fixed.
  name = "virtual_cube_no_imports",
  srcs = glob(["*.py"]),
  data = glob(["assets/*"]),
  visibility = ["//simulator/virtual_cube/tests:__pkg__"],
)
