cc_library(
  name = "mocks",
  copts = ["-Iexternal/gtest/googlemock/include"],
  hdrs = glob(["mock_*.h"]),
  deps = ["//apps/libmc/core",
          "@gtest//:gmock"],
  restricted_to = ["//cpus:x86"],
  visibility = ["//apps/libmc/sim/tests:__pkg__"],
)

cc_test(
  name = "system_event_dispatcher_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include",
           "-Iexternal/"],
  srcs = ["system_event_dispatcher_test.cc"],
  deps = ["//apps/libmc/core/events",
          "@gtest//:gtest",
          "@gtest//:gmock",
          "@tachyon//:tachyon_test"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)

cc_test(
  name = "system_event_listener_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include",
           "-Iexternal/"],
  srcs = ["system_event_listener_test.cc"],
  deps = ["//apps/libmc/core/events",
          "@gtest//:gtest",
          "@gtest//:gmock",
          "@tachyon//:tachyon_test"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)
