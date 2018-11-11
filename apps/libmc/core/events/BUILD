cc_library(
  name = "events",
  srcs = glob(["*.cc"]),
  hdrs = glob(["*.h"]),
  deps = ["//apps/libmc/core",
          "//apps/libmc/sim/protobuf:cc_system_message",
          "@com_google_protobuf//:protobuf_lite"],
  visibility = ["//apps/libmc:__subpackages__"],
)
