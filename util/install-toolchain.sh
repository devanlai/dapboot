#!/bin/bash
set -eo pipefail

if [ `uname` == 'Darwin' ]; then
TOOLCHAIN_ARCH=mac
else
TOOLCHAIN_ARCH=x86_64-linux
fi
TOOLCHAIN_REV=9-2019q4
TOOLCHAIN=gcc-arm-none-eabi-9-2019-q4-major
URL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/${TOOLCHAIN_REV}/${TOOLCHAIN}-${TOOLCHAIN_ARCH}.tar.bz2
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

EXISTING_TOOLCHAIN=`readlink "${TOOLCHAINS}/gcc-arm-embedded"`
echo "Current toolchain is $EXISTING_TOOLCHAIN"

if [ $TOOLCHAIN_ARCH != 'mac' ]; then
if ! ldd ${GCC} >/dev/null; then
    echo "${GCC} does not appear to be executable on this machine"
    exit 1
fi;
fi;

TOOLCHAIN_VER=`${GCC} --version | head -n 1`
echo "Installed toolchain version is $TOOLCHAIN_VER"
