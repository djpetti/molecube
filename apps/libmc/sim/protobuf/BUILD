package(default_visibility=["//apps/libmc/sim:__subpackages__"])

proto_library(
  name = "system_action",
  srcs = ["system_action.proto"],
)

cc_proto_library(
  name = "cc_system_action",
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
