package(default_visibility=["//apps/libmc:__subpackages__",
                            "//simulator/virtual_cube:__subpackages__"])
load("@build_stack_rules_proto//python:python_proto_library.bzl",
     "python_proto_library")

proto_library(
  name = "sim_message",
  srcs = ["sim_message.proto"],
  deps = [":system_message"],
)

cc_proto_library(
  name = "cc_sim_message",
  deps = [":sim_message"],
)

python_proto_library(
  name = "python_sim_message",
  deps = [":sim_message"],
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
  deps = [":sim_message",
          ":test"],
)

proto_library(
  name = "system_message",
  srcs = ["system_message.proto"],
)

cc_proto_library(
  name = "cc_system_message",
  deps = [":system_message"],
)
