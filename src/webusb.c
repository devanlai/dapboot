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

#include <libopencm3/usb/usbd.h>
#include "webusb.h"

#include "config.h"
#include "usb_conf.h"

#ifndef LANDING_PAGE_URL
#define LANDING_PAGE_URL "devanlai.github.io/webdfu/dfu-util/"
#endif

#define LANDING_PAGE_DESCRIPTOR_SIZE (WEBUSB_DT_URL_DESCRIPTOR_SIZE \
                                    + sizeof(LANDING_PAGE_URL) - 1)

_Static_assert((LANDING_PAGE_DESCRIPTOR_SIZE < 256),
               "Landing page URL is too long");

const struct webusb_platform_descriptor webusb_platform = {
    .bLength = WEBUSB_PLATFORM_DESCRIPTOR_SIZE,
    .bDescriptorType = USB_DT_DEVICE_CAPABILITY,
    .bDevCapabilityType = USB_DC_PLATFORM,
    .bReserved = 0,
    .platformCapabilityUUID = WEBUSB_UUID,
    .bcdVersion = 0x0100,
    .bVendorCode = WEBUSB_VENDOR_CODE,
    .iLandingPage = 1
};

static const struct webusb_url_descriptor landing_url_descriptor = {
    .bLength = LANDING_PAGE_DESCRIPTOR_SIZE,
    .bDescriptorType = WEBUSB_DT_URL,
    .bScheme = WEBUSB_URL_SCHEME_HTTPS,
    .URL = LANDING_PAGE_URL
};

static enum usbd_request_return_codes
webusb_control_vendor_request(usbd_device *usbd_dev,
                              struct usb_setup_data *req,
                              uint8_t **buf, uint16_t *len,
                              usbd_control_complete_callback* complete) {
    (void)complete;
    (void)usbd_dev;

    if (req->bRequest != WEBUSB_VENDOR_CODE) {
        return USBD_REQ_NEXT_CALLBACK;
    }

    enum usbd_request_return_codes status = USBD_REQ_NOTSUPP;
    switch (req->wIndex) {
        case WEBUSB_REQ_GET_URL: {
            if (req->wValue != 1) {
                break;
            }
            *buf = (uint8_t*)(&landing_url_descriptor);
            *len = landing_url_descriptor.bLength;
            status = USBD_REQ_HANDLED;
            break;
        }
        default: {
            status = USBD_REQ_NOTSUPP;
            break;
        }
    }

    return status;
}

static void webusb_set_config(usbd_device* usbd_dev, uint16_t wValue) {
    (void)wValue;
    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        webusb_control_vendor_request);
}

void webusb_setup(usbd_device* usbd_dev) {
    usbd_register_set_config_callback(usbd_dev, webusb_set_config);
}
