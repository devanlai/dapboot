#!/bin/bash
set -eo pipefail
URL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
TOOLCHAIN=gcc-arm-none-eabi-9-2019-q4-major
TOOLCHAINS=$HOME/toolchains
TOOLCHAIN_MISSING=0
GCC=${TOOLCHAINS}/gcc-arm-embedded/bin/arm-none-eabi-gcc
if [[ ! -d "${TOOLCHAINS}/gcc-arm-embedded" ]]; then
    TOOLCHAIN_MISSING=1
fi;
if [[ ! -f ${GCC} ]]; then
    TOOLCHAIN_MISSING=1
fi;

if [ $TOOLCHAIN_MISSING -eq 1 ]; then
    echo "Installing $TOOLCHAIN from $URL to ${TOOLCHAINS}"
    mkdir -p ${TOOLCHAINS}
    wget -qO- $URL | tar xj -C ${TOOLCHAINS}
    rm -rf ${TOOLCHAINS}/gcc-arm-embedded
    ln -s $TOOLCHAIN ${TOOLCHAINS}/gcc-arm-embedded
fi;

EXISTING_TOOLCHAIN=`readlink -f "${TOOLCHAINS}/gcc-arm-embedded"`
echo "Current toolchain is $EXISTING_TOOLCHAIN"

if ! ldd ${GCC} >/dev/null; then
    echo "${GCC} does not appear to be executable on this machine"
    exit 1
fi;

TOOLCHAIN_VER=`${GCC} --version | head -n 1`
echo "Installed toolchain version is $TOOLCHAIN_VER"
