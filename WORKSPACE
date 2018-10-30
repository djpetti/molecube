new_http_archive(
  name = "arm_gcc_linux_toolchain",
  build_file = "compilers/gcc-arm-linux-gnueabihf-8.2.BUILD",
  urls = ["https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2018.08/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf.tar.xz?revision=51f3ba22-a569-4dda-aedc-7988690c3c17&ln=en"],
  strip_prefix = "gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf"
)

# proto_library, cc_proto_library, and java_proto_library rules implicitly
# depend on @com_google_protobuf for protoc and proto runtimes.
# This statement defines the @com_google_protobuf repo.
http_archive(
    name = "com_google_protobuf",
    sha256 = "cef7f1b5a7c5fba672bec2a319246e8feba471f04dcebfe362d55930ee7c1c30",
    strip_prefix = "protobuf-3.5.0",
    urls = ["https://github.com/google/protobuf/archive/v3.5.0.zip"],
)

new_http_archive(
  name = "gtest",
  url = "https://github.com/google/googletest/archive/release-1.8.0.zip",
  sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
  build_file = "gtest.BUILD",
  strip_prefix = "googletest-release-1.8.0",
)
