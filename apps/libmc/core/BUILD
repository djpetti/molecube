cc_library(
  name = "core",
  srcs = glob(["*.cc"], exclude=["*_main.cc"]),
  hdrs = glob(["*.h"]),
  visibility = ["//visibility:public"],
  deps = ["//apps/libmc/core/events",
          "@com_google_glog//:glog",
          "@tachyon//:tachyon"],
  copts = ["-Iexternal/"],
)

cc_binary(
  name = "system_manager_process",
  srcs = ["system_manager_process_main.cc"],
  deps = [":core",
          "//apps/libmc:constants",
          "//apps/libmc/core/events",
          "@tachyon//:tachyon"],
  copts = ["-Iexternal/"],
  visibility = ["//simulator:__subpackages__"],
)
