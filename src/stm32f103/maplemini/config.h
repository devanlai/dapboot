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

#define APP_BASE_ADDRESS 0x08002000
#define FLASH_PAGE_SIZE  1024
#define DFU_UPLOAD_AVAILABLE 1
#define DFU_DOWNLOAD_AVAILABLE 1

#define HAVE_LED 1
#define LED_GPIO_PORT GPIOB
#define LED_GPIO_PIN  GPIO1
#define LED_OPEN_DRAIN 0

/* Technically, there is a button on PB8, but the button is
   also shorted to BOOT0, so it's not very useful for us to
   sample PB8 on boot, since pulling it high will already
   trigger the ROM serial bootloader and prevent us from
   running anyways. */
#define HAVE_BUTTON 0

#define HAVE_USB_PULLUP_CONTROL 1
#define USB_PULLUP_GPIO_PORT GPIOB
#define USB_PULLUP_GPIO_PIN  GPIO9
#define USB_PULLUP_ACTIVE_HIGH 0
#define USB_PULLUP_OPEN_DRAIN 1

#define USES_GPIOA 0
#define USES_GPIOB 1
#define USES_GPIOC 0

#endif
