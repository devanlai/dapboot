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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <libopencm3/usb/dfu.h>
#include "target.h"
#include "webusb.h"

#include "config.h"
#include "usb_conf.h"
#include "dfu.h"

static const struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0210,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0111,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

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
#define ALTW(n) ALT##n // Wrapper macro for expansion
#define ALTN(n) ALTW(n)

static const struct usb_interface_descriptor altsettings[] = {
    ALTN(USB_DFU_ALTN) // Prepend USB_DFU_ALTN dummy USB interface descriptors to "pad" the real one
    {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = INTF_DFU,
        .bAlternateSetting = USB_DFU_ALTN,
        .bNumEndpoints = 0,
        .bInterfaceClass = 0xFE,
        .bInterfaceSubClass = 1,
        .bInterfaceProtocol = 2,
        .iInterface = 4,

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
        .num_altsetting = USB_DFU_ALTN + 1,
        .altsetting = (const struct usb_interface_descriptor*)&altsettings,
    }
};

static const struct usb_config_descriptor config = {
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

static const struct usb_device_capability_descriptor* capabilities[] = {
    (const struct usb_device_capability_descriptor*)&webusb_platform,
};

static const struct usb_bos_descriptor bos = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .wTotalLength = USB_DT_BOS_SIZE + sizeof(webusb_platform),
    .bNumDeviceCaps = sizeof(capabilities)/sizeof(capabilities[0]),
    .capabilities = capabilities
};

static char serial_number[USB_SERIAL_NUM_LENGTH+1];

static const char *usb_strings[] = {
    USB_VENDOR_STRING,
    USB_PRODUCT_STRING,
    serial_number,
    USB_INTERFACE_STRING 
};

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[USB_CONTROL_BUF_SIZE] __attribute__ ((aligned (2)));

void usb_set_serial_number(const char* serial) {
    serial_number[0] = '\0';
    if (serial) {
        strncpy(serial_number, serial, USB_SERIAL_NUM_LENGTH);
        serial_number[USB_SERIAL_NUM_LENGTH] = '\0';
    }
}

usbd_device* usb_setup(void) {
    int num_strings = sizeof(usb_strings)/sizeof(const char*);

    const usbd_driver* driver = target_usb_init();
    usbd_device* usbd_dev = usbd_init(driver, &dev, &config, &bos,
                                      usb_strings, num_strings,
                                      usbd_control_buffer, sizeof(usbd_control_buffer));

    return usbd_dev;
}
