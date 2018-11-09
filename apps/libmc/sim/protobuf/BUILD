package(default_visibility=["//apps/libmc/sim:__subpackages__",
                            "//simulator/virtual_cube:__subpackages__"])
load("@build_stack_rules_proto//python:python_proto_library.bzl",
     "python_proto_library")

proto_library(
  name = "system_action",
  srcs = ["system_action.proto"],
)

cc_proto_library(
  name = "cc_system_action",
  deps = [":system_action"],
)

python_proto_library(
  name = "python_system_action",
  deps = [":system_action"],
)

proto_library(
  name = "test",
  srcs = ["test.proto"],
)

cc_proto_library(
  name = "cc_test",
  deps = [":test"],
)

python_proto_library(
  name = "all",
  deps = [":system_action",
          ":test"],
)
