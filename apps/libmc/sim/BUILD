cc_library(
  name = "sim",
  srcs = glob(["*.cc"], exclude=["*_main.cc"]),
  hdrs = glob(["*.h"]),
  deps = ["//apps/libmc:constants",
          "//apps/libmc/core/events",
          "//apps/libmc/sim/protobuf:cc_sim_message",
          "//apps/libmc/sim/protobuf:cc_system_message",
          "@com_google_protobuf//:protobuf_lite",
          "@com_google_glog//:glog"],
  visibility = ["//apps/libmc:__subpackages__"],
)

cc_binary(
  name = "simulator_process",
  srcs = ["simulator_process_main.cc"],
  deps = [":sim",
          "@com_github_gflags_gflags//:gflags",
          "@com_google_glog//:glog"],
)
