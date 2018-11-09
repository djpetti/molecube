py_test(
  name = "test_cube_vm",
  srcs = ["test_cube_vm.py"],
  deps = ["//simulator/virtual_cube"],
  size = "small",
)

py_test(
  name = "test_cows",
  srcs = ["test_cows.py"],
  deps = ["//simulator/virtual_cube"],
  size = "small",
)

py_test(
  name = "test_serial_com",
  srcs = ["test_serial_com.py"],
  deps = ["//simulator/virtual_cube:virtual_cube_no_imports",
          "//apps/libmc/sim/protobuf:all"],
  size = "small",
)
