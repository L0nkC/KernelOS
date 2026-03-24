# Dockerfile for building KernelOS ISO
# Uses Ubuntu with i686 cross-compiler tools

FROM ubuntu:22.04

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    binutils \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Default command is to build
CMD ["/bin/bash"]
