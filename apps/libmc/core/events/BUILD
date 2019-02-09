cc_library(
  name = "events",
  srcs = glob(["*.cc"]),
  hdrs = glob(["*.h"]),
  deps = ["//apps/libmc:constants",
          "//apps/libmc/sim/protobuf:cc_sim_message",
          "//apps/libmc/sim/protobuf:cc_system_message",
          "//apps/libmc/sim/protobuf:cc_graphics_message",
          "@com_google_protobuf//:protobuf_lite",
          "@com_google_glog//:glog",
          "@tachyon//:tachyon"],
  copts = ["-Iexternal/"],
  visibility = ["//apps/libmc:__subpackages__"],
)
