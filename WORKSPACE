new_http_archive(
  name = "arm_gcc_linux_toolchain",
  build_file = "compilers/gcc-arm-linux-gnueabihf-7.3.1.BUILD",
  urls = ["https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf.tar.xz"],
  strip_prefix = "gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf"
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

# Include Python protobuf rules.
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "build_stack_rules_proto",
  urls = ["https://github.com/stackb/rules_proto/archive/1d6550fc2e62.tar.gz"],
  sha256 = "113e6792f5b20679285c86d91c163cc8c4d2b4d24d7a087ae4f233b5d9311012",
  strip_prefix = "rules_proto-1d6550fc2e625d47dc4faadac92d7cb20e3ba5c5",
)

load("@build_stack_rules_proto//python:deps.bzl", "python_proto_library")

python_proto_library()

load("@io_bazel_rules_python//python:pip.bzl", "pip_repositories", "pip_import")

pip_repositories()

pip_import(
  name = "protobuf_py_deps",
  requirements = "@build_stack_rules_proto//python/requirements:protobuf.txt",
)

load("@protobuf_py_deps//:requirements.bzl", protobuf_pip_install = "pip_install")

protobuf_pip_install()

# Gflags git repo.
http_archive(
  name = "com_github_gflags_gflags",
  strip_prefix = "gflags-2.2.2",
  urls = [
    "https://mirror.bazel.build/github.com/gflags/gflags/archive/v2.2.2.tar.gz",
    "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
  ],
)

# GLog git repo.
git_repository(
  name = "com_google_glog",
  remote = "https://github.com/google/glog",
  commit = "781096619d3dd368cfebd33889e417a168493ce7",
)

# Tachyon.
git_repository(
  name = "tachyon",
  remote = "https://github.com/djpetti/tachyon",
  commit = "141f7252e33a8f427f3455832ead241a308ed3eb",
)
