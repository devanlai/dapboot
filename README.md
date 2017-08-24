# dapboot
The dapboot project is an open-source USB [Device Firmware Upgrade](http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf) (DFU) bootloader for STM32 devices.

Currently, the only targets officially supported are the STM32F103x series.

## Build instructions
The default target is a generic STM32F103 dev board with an LED on PC13, commonly referred to as a "bluepill" board.

To build other targets, you can override the
`TARGET` variable when invoking `make`.

    make clean
    make TARGET=STLINK

### Targets

| Target Name | Description | Link |
| ----------- | ----------- |----- |
|`BLUEPILL`   | Cheap dev board | http://wiki.stm32duino.com/index.php?title=Blue_Pill |
|`MAPLEMINI`  | LeafLabs Maple Mini board and clone derivatives | http://wiki.stm32duino.com/index.php?title=Maple_Mini |
|`STLINK`     | STLink/v2 hardware clones | https://wiki.paparazziuav.org/wiki/STLink#Clones |

## Flash instructions
The `make flash` target will use openocd to upload the bootloader to an attached board. By default, the Makefile assumes you're using a [CMSIS-DAP](http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php) based probe, but you can override this by overriding `OOCD_INTERFACE` variable. For example:

    make OOCD_INTERFACE=interface/stlink-v2.cfg flash

## Overriding defaults
Local makefile settings can be set by creating a `local.mk`, which is automatically included.

Here is an example `local.mk` that changes the default target to the STLink/v2 and uses an unmodified STLink/v2 to flash it.

    TARGET ?= STLINK
    OOCD_INTERFACE ?= interface/stlink-v2.cfg

## Using the bootloader
### Building for the bootloader
The bootloader occupies the lower 8KiB of flash, so your application must offset its flash contents by 8KiB. This can be done by modifying your linker script or flags as appropriate.

See the [highboot branch](https://github.com/devanlai/dapboot/tree/highboot) for an experimental variation that doesn't require the application to be offset.

### Switching to the bootloader
The bootloader can be built to look for arbitrary patterns, but the default for the STM32F103 target looks for a magic value stored in the RTC backup registers. Writing the magic value and then resetting will run the bootloader instead of the main application.

The bootloader currently looks for `0x544F` in RTC backup register 1 and `0x4F42` in RTC backup register 0 (together they spell "BOOT" in ASCII).

### WebUSB
This bootloader implements the draft [WebUSB](https://wicg.github.io/webusb/) specification, which allows web pages to access the bootloader (after presenting the user with a device picker dialog).

For a demo implementing dfu-util features in the browser, see https://devanlai.github.io/webdfu/dfu-util/

## USB VID/PID
The USB VID/PID pair ([1209/DB42](http://pid.codes/1209/DB42/)) is allocated through the [pid.codes](http://pid.codes/) open-source USB PID program.

## Licensing
All contents of the dapboot project are licensed under terms that are compatible with the terms of the GNU Lesser General Public License version 3.

Non-libopencm3 related portions of the dapboot project are licensed under the less restrictive ISC license, except where otherwise specified in the headers of specific files.

See the LICENSE file for full details.
