# dapboot
The dapboot project is an open-source USB [Device Firmware Upgrade](http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf) (DFU) bootloader for STM32 devices.

Currently, the only target officially supported is the STM32F103x chip on STLink/v2 compatible boards.

## Build instructions
As only one target is really in use, invoking `make` will build a binary for the STM32F103. The more general usage is:

    make clean
    make TARGET=STM32F103

## Flash instructions
The `make flash` target will use openocd to upload the bootloader to an attached board. By default, the Makefile assumes you're using a [CMSIS-DAP](http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php) based probe, but you can override this by overriding `OOCD_INTERFACE` variable. For example:

    make OOCD_INTERFACE=interface/stlink-v2.cfg flash

## Overriding defaults
Local makefile settings can be set by creating a `local.mk`, which is automatically included.

## Switching to the bootloader
The bootloader can be built to look for arbitrary patterns, but the default for the STM32F103 target looks for a magic value stored in the RTC backup registers. Writing the magic value and then resetting will run the bootloader instead of the main application.

## Licensing
All contents of the dapboot project are licensed under terms that are compatible with the terms of the GNU Lesser General Public License version 3.

Non-libopencm3 related portions of the dapboot project are licensed under the less restrictive ISC license, except where otherwise specified in the headers of specific files.

See the LICENSE file for full details.