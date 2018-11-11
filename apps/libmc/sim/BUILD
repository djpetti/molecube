cc_library(
  name = "sim",
  srcs = glob(["*.cc"]),
  hdrs = glob(["*.h"]),
  deps = ["//apps/libmc:constants",
          "//apps/libmc/core/events",
          "//apps/libmc/sim/protobuf:cc_sim_message",
          "//apps/libmc/sim/protobuf:cc_system_message",
          "@com_google_protobuf//:protobuf_lite"],
  visibility = ["//apps/libmc:__subpackages__"],
)
