/*
 * Copyright (c) 2016, Devan Lai
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
#define FLASH_PAGE_SIZE  1024
#endif
#ifndef DFU_UPLOAD_AVAILABLE
#define DFU_UPLOAD_AVAILABLE 1
#endif
#ifndef DFU_DOWNLOAD_AVAILABLE
#define DFU_DOWNLOAD_AVAILABLE 1
#endif

#ifndef HAVE_LED
#define HAVE_LED 1
#endif
#ifndef LED_ACTIVE_HIGH
#define LED_ACTIVE_HIGH 0
#endif
#ifndef LED_GPIO_PORT
#define LED_GPIO_PORT GPIOB
#endif
#ifndef LED_GPIO_PIN
#define LED_GPIO_PIN  GPIO1
#endif
#ifndef LED_OPEN_DRAIN
#define LED_OPEN_DRAIN 0
#endif

/* Technically, there is a button on PB8, but the button is
   also shorted to BOOT0, so it's not very useful for us to
   sample PB8 on boot, since pulling it high will already
   trigger the ROM serial bootloader and prevent us from
   running anyways. */
#ifndef HAVE_BUTTON
#define HAVE_BUTTON 0
#endif

#ifndef HAVE_USB_PULLUP_CONTROL
#define HAVE_USB_PULLUP_CONTROL 1
#endif
#ifndef USB_PULLUP_GPIO_PORT
#define USB_PULLUP_GPIO_PORT GPIOB
#endif
#ifndef USB_PULLUP_GPIO_PIN
#define USB_PULLUP_GPIO_PIN  GPIO9
#endif
#ifndef USB_PULLUP_ACTIVE_HIGH
#define USB_PULLUP_ACTIVE_HIGH 0
#endif
#ifndef USB_PULLUP_OPEN_DRAIN
#define USB_PULLUP_OPEN_DRAIN 1
#endif

#ifndef USES_GPIOA
#define USES_GPIOA 0
#endif
#ifndef USES_GPIOB
#define USES_GPIOB 1
#endif
#ifndef USES_GPIOC
#define USES_GPIOC 0
#endif


#ifndef USES_HSE_12M
#define USES_HSE_12M 0
#endif

#endif
