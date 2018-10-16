# This is the entry point for --crosstool_top.
#
# The cc_toolchain rule used is found by:
#
# 1. Finding the appropriate toolchain in the CROSSTOOL file based on the --cpu
#    and --compiler command line flags (if they exist, otherwise using the
#    "default_target_cpu" / "default_toolchain" fields in the CROSSTOOL file)
# 2. Concatenating the "target_cpu" and "compiler" fields of the toolchain in
#    use and using that as a key in the map in the "toolchains" attribute
cc_toolchain_suite(
  name = "toolchain",
  toolchains = {
    "armeabi-v7a|gcc": ":cc-compiler-armeabi-v7a",
  },
  visibility = ["//visibility:public"],
)

filegroup(
  name = 'arm_linux_all_files',
  srcs = [
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:tool-wrappers',
    '@arm_gcc_linux_toolchain//:compiler_pieces',
  ],
)

filegroup(
  name = 'arm_linux_linker_files',
  srcs = [
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:gcc',
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:ld',
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:ar',
    '@arm_gcc_linux_toolchain//:compiler_pieces',
  ],
)

filegroup(
  name = 'arm_linux_compiler_files',
  srcs = [
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:gcc',
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:ld',
    '//tools/arm_compiler/gcc_arm_linux_gnueabihf:as',
  ],
)

cc_toolchain(
  name = 'cc-compiler-armeabi-v7a',
  all_files = ':arm_linux_all_files',
  compiler_files = ':arm_linux_compiler_files',
  cpu = 'armeabi-v7a',
  dwp_files = ':empty',
  dynamic_runtime_libs = [':empty'],
  linker_files = ':arm_linux_linker_files',
  objcopy_files = '//tools/arm_compiler/gcc_arm_linux_gnueabihf:objcopy',
  static_runtime_libs = [':empty'],
  strip_files = '//tools/arm_compiler/gcc_arm_linux_gnueabihf:strip',
  supports_param_files = 1,
	visibility = ["//visibility:public"],
)
