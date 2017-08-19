#!/bin/bash
URL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2
TOOLCHAIN=gcc-arm-none-eabi-6-2017-q2-update
if [[ ! -d "$HOME/toolchains/gcc-arm-embedded" ]]; then
    mkdir -p ~/toolchains
    wget -qO- $URL | tar xj -C ~/toolchains/
    ln -s $TOOLCHAIN ~/toolchains/gcc-arm-embedded
fi;
