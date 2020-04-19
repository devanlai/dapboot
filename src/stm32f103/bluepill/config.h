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
#define APP_BASE_ADDRESS 0x08000000
#endif
#ifndef FLASH_SIZE_OVERRIDE
#define FLASH_SIZE_OVERRIDE 0x20000
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
#ifndef LED_OPEN_DRAIN
#define LED_OPEN_DRAIN 1
#endif
#ifndef LED_GPIO_PORT
#define LED_GPIO_PORT GPIOC
#endif
#ifndef LED_GPIO_PIN
#define LED_GPIO_PIN GPIO13
#endif

#ifndef HAVE_BUTTON
#define HAVE_BUTTON 1
#endif
#ifndef BUTTON_ACTIVE_HIGH
#define BUTTON_ACTIVE_HIGH 1
#endif
#ifndef BUTTON_GPIO_PORT
#define BUTTON_GPIO_PORT GPIOB
#endif
#ifndef BUTTON_GPIO_PIN
#define BUTTON_GPIO_PIN GPIO2
#endif
// Blue-Pull has 100k resistors on PB2, so we can't use weak pulls to read it.
#ifndef BUTTON_USES_PULL
#define BUTTON_USES_PULL 0
#endif

#ifndef BUTTON_SAMPLE_DELAY_CYCLES
#define BUTTON_SAMPLE_DELAY_CYCLES 1440000
#endif

#ifndef HAVE_USB_PULLUP_CONTROL
#define HAVE_USB_PULLUP_CONTROL 0
#endif

#ifndef USES_GPIOB
#define USES_GPIOB 1
#endif

#ifndef USES_GPIOC
#define USES_GPIOC 1
#endif

#endif
