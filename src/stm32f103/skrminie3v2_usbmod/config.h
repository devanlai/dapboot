/*
 * Copyright (c) 2021, Dennis Marttinen
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * ATTENTION! Using the USBMOD target requires hardware modifications to your BTT SKR MINI E3 V2.0. These modifications
 * aim to restore the broken SWD debugging support of this board by removing "unnecessary" components attached to the
 * SWDIO and SWDCLK pins. Read the comments carefully, and proceed at your own risk! Flashing this firmware on an
 * unmodified SKR MINI will not harm it, but USB will not work in the bootloader. Here's the schematic for the board for
 * looking up the component identifiers mentioned in the comments:
 * https://github.com/bigtreetech/BIGTREETECH-SKR-mini-E3/blob/master/hardware/BTT%20SKR%20MINI%20E3%20V2.0/Hardware/BTT%20SKR%20MINI%20E3%20V2.0SCHpdf.PDF
 */

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifndef APP_BASE_ADDRESS
#define APP_BASE_ADDRESS (0x08000000 + BOOTLOADER_OFFSET)
#endif
#ifndef FLASH_PAGE_SIZE
/* The BTT SKR MINI E3 V2.0 uses an STM32F103RC with 256 KiB of flash and 2 KiB pages */
#define FLASH_PAGE_SIZE  2048
#endif
#ifndef DFU_UPLOAD_AVAILABLE
#define DFU_UPLOAD_AVAILABLE 1
#endif
#ifndef DFU_DOWNLOAD_AVAILABLE
#define DFU_DOWNLOAD_AVAILABLE 1
#endif
#ifndef DFU_WILL_DETACH
#define DFU_WILL_DETACH 0
#endif

/* There is a PCB-mounted status LED, but it's unreadable when the board is installed, and additionally it's wired to
 * SWDIO so toggling it manually is a bad idea. To restore reliable SWD communication, desolder either the status LED
 * itself (D16) or the resistor for it (R90). */
#ifndef HAVE_LED
#define HAVE_LED 0
#endif

/* Display encoder button (BTN-ENC) on PA15, no external pullup */
#ifndef HAVE_BUTTON
#define HAVE_BUTTON 1
#endif
#ifndef BUTTON_ACTIVE_HIGH
#define BUTTON_ACTIVE_HIGH 0
#endif
#ifndef BUTTON_GPIO_PORT
#define BUTTON_GPIO_PORT GPIOA
#endif
#ifndef BUTTON_GPIO_PIN
#define BUTTON_GPIO_PIN GPIO15
#endif
#ifndef BUTTON_USES_PULL
#define BUTTON_USES_PULL 1
#endif
#ifndef BUTTON_SAMPLE_DELAY_CYCLES
#define BUTTON_SAMPLE_DELAY_CYCLES 1440000
#endif

/* The USB pullup MOSFET (U7) with its pullup resistor (R43) is wired to SWCLK on this board which makes attaching a
 * debug probe nearly impossible. It is however possible to desolder these two components and bridge the source and
 * drain contact pads for the MOSFET to gain debugger support. The [datasheet] for the MCU suggests that the pullup
 * resistor for the USB D+ pin (R44) is enough on its own, as the MCU itself can pull the pin down to initiate a USB
 * reset without an external MOSFET controlling that resistor. This does indeed work without any configuration in
 * dapboot as well as both the Klipper and Marlin 3D printer firmwares. There were no problems during testing when
 * performing software resets or using the reset button, the USB reset works seamlessly even when transitioning from
 * dapboot to a freshly flashed firmware.
 * [datasheet]: https://www.st.com/resource/en/datasheet/stm32f103rc.pdf */
#ifndef HAVE_USB_PULLUP_CONTROL
#define HAVE_USB_PULLUP_CONTROL 0
#endif

#ifndef USES_GPIOA
#define USES_GPIOA 1
#endif

/* For stm32duino bootloader compatibility, the following options enable
 * bootloader flashing using KIAUH: https://github.com/th33xitus/kiauh */
#ifndef REG_BOOT
#define REG_BOOT BKP10
#endif

#ifndef CMD_BOOT
#define CMD_BOOT 1
#endif

#ifndef USB_DFU_ALTN
#define USB_DFU_ALTN 2
#endif

#endif
