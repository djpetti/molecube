py_library(
  name = "virtual_cube",
  srcs = glob(["*.py"], exclude=["starter.py"]),
  # We need access to all the cube binaries so we can make them accessible in
  # the VM,
  data = glob(["assets/*"]) + \
         [":starter",
          "//apps/libmc/sim:simulator_process",
          "//apps/libmc/core:system_manager_process"],
  deps = ["//apps/libmc/sim/protobuf:python_sim_message",
          "//config"],
  visibility = ["//simulator:__subpackages__"],
)

py_library(
  # TODO (danielp) This target is a really hacky workaround for a bug in the
  # Python protobuf rules that causes it to add duplicate entries to sys.path.
  # Change this as soon as it gets fixed.
  name = "virtual_cube_no_imports",
  srcs = glob(["*.py"], exclude=["starter.py"]),
  data = glob(["assets/*"]),
  deps = ["//config"],
  visibility = ["//simulator/virtual_cube/tests:__pkg__"],
)

py_binary(
  name = "starter",
  srcs = ["starter.py"],
)
