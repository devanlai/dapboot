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

#ifndef USB_CONF_H_INCLUDED
#define USB_CONF_H_INCLUDED

#include <libopencm3/usb/usbd.h>

#ifndef USB_VID
#define USB_VID                 0x1209
#endif

#ifndef USB_VENDOR_STRING
#define USB_VENDOR_STRING "Devanarchy"
#endif

#ifndef USB_PID
#define USB_PID                 0xdb42
#endif

// The DFU interface doesn't have any special alternate setting by default
#ifndef USB_DFU_ALTN
#define USB_DFU_ALTN 0
#endif

#ifndef USB_PRODUCT_STRING

#ifdef BOOTLOADER_HIGH
#define USB_PRODUCT_STRING "DAPBoot High-Memory DFU Bootloader"
#else
#define USB_PRODUCT_STRING "DAPBoot DFU Bootloader"
#endif
 
#endif

#ifndef USB_INTERFACE_STRING
#define USB_INTERFACE_STRING "DAPBoot DFU"
#endif

#define USB_CONTROL_BUF_SIZE    1024
#define USB_SERIAL_NUM_LENGTH   24
#define INTF_DFU                0

extern void usb_set_serial_number(const char* serial);
extern usbd_device* usb_setup(void);

#endif
