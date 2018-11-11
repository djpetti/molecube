cc_library(
  name = "mocks",
  copts = ["-Iexternal/gtest/googlemock/include"],
  hdrs = glob(["mock_*.h"]),
  deps = ["//apps/libmc/sim",
          "@gtest//:gmock"],
  restricted_to = ["//cpus:x86"],
)

cc_test(
  name = "cows_test",
  copts = ["-Iexternal/gtest/googletest/include"],
  srcs = ["cows_test.cc"],
  deps = ["//apps/libmc/sim",
          "@gtest//:gtest"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)

cc_test(
  name = "serial_link_pty_test",
  copts = ["-Iexternal/gtest/googletest/include"],
  srcs = ["serial_link_pty_test.cc"],
  deps = ["//apps/libmc/sim",
          "@gtest//:gtest"],
  size = "small",
  restricted_to = ["//cpus:x86"],
  # Disable sandboxing so we can access the Linux PTY subsystem.
  tags = ["local"],
)

cc_test(
  name = "serial_link_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include"],
  srcs = ["serial_link_test.cc"],
  deps = [":mocks",
          "//apps/libmc/sim",
          "@gtest//:gtest",
          "@gtest//:gmock"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)

cc_test(
  name = "simulator_com_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include"],
  srcs = ["simulator_com_test.cc"],
  deps = [":mocks",
          "//apps/libmc/sim",
          "//apps/libmc/sim/protobuf:cc_test",
          "@gtest//:gtest",
          "@gtest//:gmock"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)

cc_test(
  name = "simulator_process_test",
  copts = ["-Iexternal/gtest/googletest/include",
           "-Iexternal/gtest/googlemock/include"],
  srcs = ["simulator_process_test.cc"],
  deps = [":mocks",
          "//apps/libmc/core/events/tests:mocks",
          "//apps/libmc/sim/protobuf:cc_sim_message",
          "//apps/libmc/sim/protobuf:cc_system_message",
          "//apps/libmc/sim",
          "@gtest//:gtest",
          "@gtest//:gmock"],
  size = "small",
  restricted_to = ["//cpus:x86"],
)
