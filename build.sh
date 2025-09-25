#!/bin/bash

echo -e "\n[INFO]: BUILD STARTED..!\n"

# init submodules
git submodule init && git submodule update

export KERNEL_ROOT="$(pwd)"
export ARCH=arm64
export KBUILD_BUILD_USER="@ravindu644"

# Create necessary directories
mkdir -p "${KERNEL_ROOT}/out" "${KERNEL_ROOT}/build" "${HOME}/toolchains"

# toolchain
export BUILD_CC=clang
export BUILD_CROSS_COMPILE=aarch64-linux-gnu-
export LD=aarch64-linux-gnu-ld

build_kernel(){
    # defconfig
    make -C ${KERNEL_ROOT} O=${KERNEL_ROOT}/out \
        ARCH=arm64 LLVM=1 LLVM_IAS=1 \
        CROSS_COMPILE=${BUILD_CROSS_COMPILE} \
        CC=${BUILD_CC} -j2 gki_defconfig

    # langsung build kernel (skip menuconfig biar nggak error terminal size)
    make -C ${KERNEL_ROOT} O=${KERNEL_ROOT}/out \
        ARCH=arm64 LLVM=1 LLVM_IAS=1 \
        CROSS_COMPILE=${BUILD_CROSS_COMPILE} \
        CC=${BUILD_CC} -j2 Image || exit 1

    # copy hasil
    cp "${KERNEL_ROOT}/out/arch/arm64/boot/Image" "${KERNEL_ROOT}/build"

    echo -e "\n[INFO]: BUILD FINISHED..!"
}
build_kernel