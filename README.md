# dapboot
The dapboot project is an open-source USB [Device Firmware Upgrade](http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf) (DFU) bootloader for STM32 devices.

Currently, the only targets officially supported are the STM32F103x series.

## Build instructions
The default target is a generic STM32F103 dev board with an LED on PC13, commonly referred to as a "bluepill" board.

To build other targets, you can override the
`TARGET` variable when invoking `make`.

    cd src/
    make clean
    make TARGET=STLINK

### Targets

| Target Name | Description | Link |
| ----------- | ----------- |----- |
|`BLUEPILL`   | Cheap dev board | https://stm32duinoforum.com/forum/wiki_subdomain/index_title_Blue_Pill.html |
|`MAPLEMINI`  | LeafLabs Maple Mini board and clone derivatives | https://stm32duinoforum.com/forum/wiki_subdomain/index_title_Maple_Mini.html |
|`STLINK`     | STLink/v2 hardware clones | https://wiki.paparazziuav.org/wiki/STLink#Clones |
|`OLIMEXSTM32H103` | Olimex STM32-H103 | https://www.olimex.com/Products/ARM/ST/STM32-H103/ |
|`BLUEPILLPLUSSTM32` | Bluepill with USB C | https://github.com/WeActTC/BluePill-Plus/ |
|`BTTSKRMINIE3V2` | BigTreeTech SKR MINI E3 V2.0 (3D printer motherboard) | https://github.com/bigtreetech/BIGTREETECH-SKR-mini-E3 |

For the above targets there are some potential variants that can be added to the target name based on what the target supports:

| Target Variant | Description                                             |
| -------------- | ------------------------------------------------------- |
|` `             | Standard bootloader, using first 8 KiB of flash         |
|`_HIGH`         | High memory bootloader for 64 KiB chips  (experimental) |
|`_HIGH_128`     | High memory bootloader for 128 KiB chips (experimental) |
|`_HIGH_256`     | High memory bootloader for 256 KiB chips (experimental) |

The high memory bootloader is a variation that doesn't require the application to be at an offset, the bootloader resides in the top 7 KiB of ROM and hides its reset and stack vectors inside unused entries of the application vector table. As an example, to compile for a Bluepill board with 128 KiB flash, use:

    make clean
    make TARGET=BLUEPILL_HIGH_128


## Flash instructions
The `make flash` target will use openocd to upload the bootloader to an attached board. By default, the Makefile assumes you're using a [CMSIS-DAP](http://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php) based probe, but you can override this by overriding `OOCD_INTERFACE` variable. For example:

    make OOCD_INTERFACE=interface/stlink-v2.cfg flash

## Overriding defaults
Local makefile settings can be set by creating a `local.mk`, which is automatically included.

Here is an example `local.mk` that changes the default target to the STLink/v2 and uses an unmodified STLink/v2 to flash it.

    TARGET ?= STLINK
    OOCD_INTERFACE ?= interface/stlink-v2.cfg

You can also use the env variable `DEFS` to override default configuration for a target, like:

    # Disable LED on BluePill
    DEFS="-DHAVE_LED=0" make TARGET=BLUEPILL

    # Allow access to all Flash on MapleMini and change the app base address
    DEFS="-DFLASH_SIZE_OVERRIDE=0x20000 -DAPP_BASE_ADDRESS=0x08004000" make TARGET=MAPLEMINI LDSCRIPT="/some/folder/stm32f103x8-16kb-boot.ld"

## Using the bootloader
### Building for the bootloader
The standard bootloader occupies the lower 8KiB of flash, so your application must offset its flash contents by 8KiB. This can be done by modifying your linker script or flags as appropriate.

The high memory bootloaders do not use the lower part of the flash, so you only need to make sure your application leaves 7 KiB of flash free.


### Switching to the bootloader
The bootloader can be built to look for arbitrary patterns, but the default looks for a magic value stored in the RTC backup registers. Writing the magic value and then resetting will run the bootloader instead of the main application.

In version v1.11 and earlier, the bootloader for STM32F103 targets looks for `0x544F` in RTC backup register 2 and `0x4F42` in RTC backup register 1 (together they spell "BOOT" in ASCII). From v1.20 on, the bootloader will only use RTC backup register 1 and check for `0x4F42` on targets with 16-bit backup registers and `0x544F4F42` on targets with 32-bit backup registers.

The backup register and bootloader command word can be customized with the `REG_BOOT` and `CMD_BOOT` defines respectively.

You can also use a button to stay in bootloader while booting. It's configured using `HAVE_BUTTON` define. If your button has a debounce capacitor, you can use `BUTTON_SAMPLE_DELAY_CYCLES` define to specify how many cycles to wait before sampling the I/O pin, by default it is approximately 20ms in a 72Mhz MCU.

On the bluepill boards, the default is using the BOOT1 input (available on jumper in the board) to enter the bootloader.

### WebUSB
This bootloader implements the draft [WebUSB](https://wicg.github.io/webusb/) specification, which allows web pages to access the bootloader (after presenting the user with a device picker dialog).

For a demo implementing dfu-util features in the browser, see https://devanlai.github.io/webdfu/dfu-util/

To customize the WebUSB landing page, you can use the `LANDING_PAGE_URL` define. To set it from the command line, you can use the `DEFS` environment variable:

    DEFS='-DLANDING_PAGE_URL=\"example.com/dfu-util/\"' make

Note that the URL scheme shoul not be part of the `LANDING_PAGE_URL` string. As of this writing, it is hardcoded to HTTPS. 

### Manifestation behavior
There are two different manifestation behaviors that can be selected using the `DFU_WILL_DETACH` define. By default, `DFU_WILL_DETACH` is set to 1 for backwards compatibility with existing behavior.

When `DFU_WILL_DETACH` is enabled, the bootloader autonomously reboot into the new firmware after it has been successfully downloaded and passes the basic validation check. This can be helpful on platforms such as Windows where it is not possible to generate a USB bus reset to signal that the bootloader should switch to the application.

When `DFU_WILL_DETACH` is disabled, the bootloader will return to the idle state after validating the firmware. The DFU host application can then perform other operations or reboot the target into the new firmware by issuing a DFU detach request or generating a USB bus reset. (This can be done with `dfu-util` by passing the `-R` flag).

## USB VID/PID
The default USB VID/PID pair ([1209/DB42](http://pid.codes/1209/DB42/)) is allocated through the [pid.codes](http://pid.codes/) open-source USB PID program.

To use a custom VID/PID pair, you need to set the macros `USB_VID` and `USB_PID`. One way to do this is by setting the `DEFS` environment variable when compiling:

     DEFS="-DUSB_VID=0x1209 -DUSB_PID=0xCAFE" make


## USB Vendor, Product, and Interface strings

To customize the USB vendor, product, and interface strings that DAPBoot reports to the USB host, you may set the macros `USB_VENDOR_STRING`, `USB_PRODUCT_STRING`, and `USB_INTERFACE_STRING`, respectively. You can do this by setting the `DEFS` environment variable or including these macros in your board's `config.h`


## Licensing
All contents of the dapboot project are licensed under terms that are compatible with the terms of the GNU Lesser General Public License version 3.

Non-libopencm3 related portions of the dapboot project are licensed under the less restrictive ISC license, except where otherwise specified in the headers of specific files.

See the LICENSE file for full details.
