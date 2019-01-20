cc_library(
  name = "mocks",
  copts = ["-Iexternal/gtest/googlemock/include"],
  hdrs = glob(["mock_*.h"]),
  deps = ["//apps/libmc/core/events",
          "@gtest//:gmock"],
)

cc_test(
  name = "system_manager_process_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include"],
  srcs = ["system_manager_process_test.cc"],
  deps = [":mocks",
          "//apps/libmc/core",
          "//apps/libmc/core/events",
          "//apps/libmc/core/events/tests:mocks",
          "@gtest//:gtest",
          "@gtest//:gmock"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)
