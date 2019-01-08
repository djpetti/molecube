FROM ubuntu:18.04

# Install packaged dependencies.
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install -y git python build-essential pkg-config zip zlib1g-dev \
                       unzip qemu-system-arm wget python-pip g++-multilib \
                       python-tk

# Install python dependencies.
RUN pip install mock pyserial pyyaml

# Install Bazel.
RUN wget -O bazel_install.sh \
    https://github.com/bazelbuild/bazel/releases/download/0.18.0/bazel-0.18.0-installer-linux-x86_64.sh
RUN chmod +x bazel_install.sh
RUN ./bazel_install.sh
