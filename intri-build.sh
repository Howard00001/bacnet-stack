#!/bin/sh

set -xe

# Set cross-compilation tools and paths
CROSS="aarch64-linux-gnu"
CROSS_PREFIX="${CROSS}-"
CROSS_CC="$(which ${CROSS_PREFIX}gcc)"
CROSS_CXX="$(which ${CROSS_PREFIX}g++)"
CROSS_LIBC="$(realpath $(dirname ${CROSS_CC})/..)/${CROSS}/libc"

# Directories
START_DIR="${PWD}"
INCLUDE_DIR="${START_DIR}/src"  # Adjust this path to your include directory if different

# Clean previous builds
make clean

# Build the BACnet server
make \
    CC="${CROSS_CC}" \
    CXX="${CROSS_CXX}" \
    CFLAGS="${CROSS_CC_FLAG} -I${INCLUDE_DIR}" \
    LDFLAGS="${CROSS_CXX_FLAG}" \
    server