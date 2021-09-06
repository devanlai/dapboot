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

#ifndef DAPBOOT_USB_DESCRIPTOR_H
#define DAPBOOT_USB_DESCRIPTOR_H

#include "usb_conf.h"
#include "dfu.h"

// Macro to create a dummy (no-op) USB interface descriptor with the given alternate setting
#define ALT_DUMMY(N) {                      \
    .bLength = USB_DT_INTERFACE_SIZE,       \
    .bDescriptorType = USB_DT_INTERFACE,    \
    .bInterfaceNumber = INTF_DFU,           \
    .bAlternateSetting = (N),               \
    .bNumEndpoints = 0,                     \
    .bInterfaceClass = 0,                   \
    .bInterfaceSubClass = 0,                \
    .bInterfaceProtocol = 0,                \
    .iInterface = 0,                        \
    .endpoint = NULL,                       \
    .extra = NULL,                          \
    .extralen = 0,                          \
},

// Functionality for creating repetitive ALT_DUMMY structs with an increasing count during compile time.
// It doesn't look very nice, but C doesn't allow loops in preprocessor macros, so this needs to be hard-coded.
#define ALT0
#define ALT1 ALT_DUMMY(0)
#define ALT2 ALT1 ALT_DUMMY(1)
#define ALT3 ALT2 ALT_DUMMY(2)
#define ALT4 ALT3 ALT_DUMMY(3)
#define ALT5 ALT4 ALT_DUMMY(4)
#define ALTC(n) ALT##n
#define ALTN(n) ALTC(n)

// The DFU interface doesn't have any special alternate setting by default
#ifndef USB_ALT
#define USB_ALT 0
#endif

static const struct usb_interface_descriptor altsettings[] = {
    ALTN(USB_ALT) // Prepend USB_ALT dummy USB interface descriptors to "pad" the real one
    {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = INTF_DFU,
        .bAlternateSetting = USB_ALT,
        .bNumEndpoints = 0,
        .bInterfaceClass = 0xFE,
        .bInterfaceSubClass = 1,
        .bInterfaceProtocol = 2,
        .iInterface = 0,

        .endpoint = NULL,

        .extra = &dfu_function,
        .extralen = sizeof(dfu_function),
    }
};

// Tracking this is mandatory if exposing multiple altsettings
static uint8_t cur_altsetting = 0;

static const struct usb_interface interfaces[] = {
    /* DFU interface */
    {
        .cur_altsetting = &cur_altsetting,
        .num_altsetting = USB_ALT + 1,
        .altsetting = (const struct usb_interface_descriptor*)&altsettings,
    },
};

static const struct usb_config_descriptor usb_config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = sizeof(interfaces)/sizeof(struct usb_interface),
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0xC0,
    .bMaxPower = 0x32,

    .interface = interfaces,
};

#endif //DAPBOOT_USB_DESCRIPTOR_H
