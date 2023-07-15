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

/* STATUS LED */
#ifndef HAVE_LED
#define HAVE_LED 1
#endif
#ifndef LED_OPEN_DRAIN
#define LED_OPEN_DRAIN 0
#endif
#ifndef LED_GPIO_PORT
#define LED_GPIO_PORT GPIOA
#endif
#ifndef LED_GPIO_PIN
#define LED_GPIO_PIN GPIO13
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

#ifndef HAVE_USB_PULLUP_CONTROL
#define HAVE_USB_PULLUP_CONTROL 1
#endif
#ifndef USB_PULLUP_GPIO_PORT
#define USB_PULLUP_GPIO_PORT GPIOA
#endif
#ifndef USB_PULLUP_GPIO_PIN
#define USB_PULLUP_GPIO_PIN  GPIO14
#endif
#ifndef USB_PULLUP_ACTIVE_HIGH
#define USB_PULLUP_ACTIVE_HIGH 0
#endif
#ifndef USB_PULLUP_OPEN_DRAIN
#define USB_PULLUP_OPEN_DRAIN 1
#endif

#ifndef USES_GPIOA
#define USES_GPIOA 1
#endif

/* For stm32duino bootloader compatibility, the following options enable bootloader flashing using Klipper:
 * https://github.com/Klipper3d/klipper/blob/6d48adf9ef5d17632acf53a7e3a07964f6cfd642/src/stm32/stm32f1.c#L238 */
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
