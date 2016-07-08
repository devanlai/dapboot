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
#include <string.h>

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dfu.h>

#include "dfu.h"
#include "usb_conf.h"
#include "dfu_defs.h"
#include "target.h"
#include "dapboot.h"
#include "config.h"

const struct usb_dfu_descriptor dfu_function = {
    .bLength = sizeof(struct usb_dfu_descriptor),
    .bDescriptorType = DFU_FUNCTIONAL,
    .bmAttributes = ((DFU_DOWNLOAD_AVAILABLE ? USB_DFU_CAN_DOWNLOAD : 0) |
                     (DFU_UPLOAD_AVAILABLE ? USB_DFU_CAN_UPLOAD : 0) |
                     USB_DFU_WILL_DETACH ),
    .wDetachTimeout = 255,
    .wTransferSize = USB_CONTROL_BUF_SIZE,
    .bcdDFUVersion = 0x0110,
};

static enum dfu_state current_dfu_state;
static enum dfu_status current_dfu_status;
static size_t current_dfu_offset;

static uint8_t dfu_download_buffer[USB_CONTROL_BUF_SIZE];
static size_t dfu_download_size;

/* User callbacks */
static GenericCallback dfu_manifest_request_callback = NULL;
static StateChangeCallback dfu_state_change_callback = NULL;
static StatusChangeCallback dfu_status_change_callback = NULL;

static inline void dfu_set_state(enum dfu_state state) {
    if (state != current_dfu_state) {
        if (dfu_state_change_callback) {
            dfu_state_change_callback(state);
        }
    }
    current_dfu_state = state;
}

static inline enum dfu_status dfu_get_status(void) {
    return current_dfu_status;
}

static inline void dfu_set_status(enum dfu_status status) {
    if (status != current_dfu_status) {
        if (dfu_status_change_callback) {
            dfu_status_change_callback(status);
        }
    }
    if (status == DFU_STATUS_OK) {
        if (current_dfu_state == STATE_DFU_ERROR) {
            dfu_set_state(STATE_DFU_IDLE);
        }
    } else {
        dfu_set_state(STATE_DFU_ERROR);
    }
    current_dfu_status = status;
}

static void dfu_on_download_complete(usbd_device* usbd_dev, struct usb_setup_data* req) {
    (void)usbd_dev;
    (void)req;
    
    dfu_set_state(STATE_DFU_MANIFEST_SYNC);
}

extern unsigned _stack;

static void dfu_on_download_request(usbd_device* usbd_dev, struct usb_setup_data* req) {
    (void)usbd_dev;
    (void)req;

    const uint16_t* data = (uint16_t*)dfu_download_buffer;
    uint16_t* dest = (uint16_t*)(APP_BASE_ADDRESS + current_dfu_offset);

    target_flash_unlock();
    bool ok = target_flash_program_array(dest, data, dfu_download_size/2);
    target_flash_lock();

    if (ok) {
        current_dfu_offset += dfu_download_size;
        /* We could go back to STATE_DFU_DNLOAD_SYNC, but then
           we would have to remember that we already programmed this block */
        dfu_set_state(STATE_DFU_DNLOAD_IDLE);
    } else {
        dfu_set_status(DFU_STATUS_ERR_VERIFY);
    }
}

static void dfu_on_manifest_request(usbd_device* usbd_dev, struct usb_setup_data* req) {
    (void)usbd_dev;
    (void)req;
    if (dfu_manifest_request_callback) {
        dfu_manifest_request_callback();
    } else {
        dfu_set_status(DFU_STATUS_ERR_UNKNOWN);
    }
}

static int dfu_control_class_request(usbd_device *usbd_dev,
                                     struct usb_setup_data *req,
                                     uint8_t **buf, uint16_t *len,
                                     usbd_control_complete_callback* complete) {
    if (req->wIndex != INTF_DFU) {
        return USBD_REQ_NEXT_CALLBACK;
    }

    int status = USBD_REQ_HANDLED;
    switch (req->bRequest) {
        case DFU_GETSTATE: {
            struct dfu_getstate_response* resp;
            resp = (struct dfu_getstate_response*)(*buf);
            resp->bState = (uint8_t)current_dfu_state;
            *len = sizeof(*resp);
            break;
        }
        case DFU_GETSTATUS: {
            struct dfu_getstatus_response* resp;
            resp = (struct dfu_getstatus_response*)(*buf);
            uint32_t bwPollTimeout = 0;
            switch (current_dfu_state) {
#if DFU_DOWNLOAD_AVAILABLE
                case STATE_DFU_DNLOAD_SYNC: {
                    dfu_set_state(STATE_DFU_DNBUSY);
                    bwPollTimeout = 100;
                    *complete = &dfu_on_download_request;
                    break;
                }
                case STATE_DFU_MANIFEST_SYNC: {
                    if (validate_application()) {
                        dfu_set_state(STATE_DFU_MANIFEST);
                        *complete = &dfu_on_manifest_request;
                    } else {
                        dfu_set_status(DFU_STATUS_ERR_FIRMWARE);
                    }
                    break;
                }
#endif
                default: {
                    break;
                }
            }
            resp->bStatus = dfu_get_status();
            resp->bwPollTimeout[0] = (uint8_t)(bwPollTimeout & 0xFF);
            resp->bwPollTimeout[1] = (uint8_t)((bwPollTimeout >> 8) & 0xFF);
            resp->bwPollTimeout[2] = (uint8_t)((bwPollTimeout >> 16) & 0xFF);
            resp->bState = (uint8_t)current_dfu_state;
            resp->iString = 0;
            *len = sizeof(*resp);
            break;
        }
        case DFU_CLRSTATUS: {
            dfu_set_status(DFU_STATUS_OK);
            break;
        }
#if DFU_DOWNLOAD_AVAILABLE
        case DFU_DNLOAD: {
            switch (current_dfu_state) {
                case STATE_DFU_IDLE: {
                    if (req->wLength > 0) {
                        current_dfu_offset = 0;
                        dfu_download_size = req->wLength;
                        memcpy(dfu_download_buffer, *buf, dfu_download_size);
                        dfu_set_state(STATE_DFU_DNLOAD_SYNC);
                    } else {
                        dfu_set_status(DFU_STATUS_ERR_STALLEDPKT);
                        usbd_ep_stall_set(usbd_dev, 0x00, 1);
                    }
                    break;
                }
                case STATE_DFU_DNLOAD_IDLE: {
                    if (req->wLength > 0) {
                        dfu_download_size = req->wLength;
                        memcpy(dfu_download_buffer, *buf, dfu_download_size);
                        dfu_set_state(STATE_DFU_DNLOAD_SYNC);
                    } else {
                        *complete = &dfu_on_download_complete;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
#endif
        case DFU_ABORT: {
            switch (current_dfu_state) {
                case STATE_DFU_IDLE:
                case STATE_DFU_DNLOAD_SYNC:
                case STATE_DFU_DNLOAD_IDLE:
                case STATE_DFU_MANIFEST_SYNC:
                case STATE_DFU_UPLOAD_IDLE: {
                    dfu_set_state(STATE_DFU_IDLE);
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
#if DFU_UPLOAD_AVAILABLE
        case DFU_UPLOAD: {
            switch (current_dfu_state) {
                case STATE_DFU_IDLE: {
                    current_dfu_offset = 0;
                    /* Fall through */
                }
                case STATE_DFU_UPLOAD_IDLE: {
                    *buf = (uint8_t*)(APP_BASE_ADDRESS + current_dfu_offset);
                    uint16_t len_to_copy = req->wLength;
                    size_t max_firmware_size = target_get_max_firmware_size();
                    if (current_dfu_offset + req->wLength > max_firmware_size) {
                        len_to_copy = max_firmware_size - current_dfu_offset;
                        dfu_set_state(STATE_DFU_IDLE);
                    } else {
                        dfu_set_state(STATE_DFU_UPLOAD_IDLE);
                    }
                    *len = len_to_copy;
                    current_dfu_offset += len_to_copy;
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
#endif
        case DFU_DETACH:
        default: {
            /* Stall the control pipe */
            dfu_set_status(DFU_STATUS_ERR_STALLEDPKT);
            usbd_ep_stall_set(usbd_dev, 0x00, 1);
            status = USBD_REQ_NOTSUPP;
            break;
        }
    }

    return status;
}

static void dfu_set_config(usbd_device* usbd_dev, uint16_t wValue) {
    (void)wValue;

    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        dfu_control_class_request);
}

void dfu_setup(usbd_device* usbd_dev,
               GenericCallback on_manifest_request,
               StateChangeCallback on_state_change,
               StatusChangeCallback on_status_change) {
    dfu_manifest_request_callback = on_manifest_request;
    dfu_state_change_callback = on_state_change;
    dfu_status_change_callback = on_status_change;

    usbd_register_set_config_callback(usbd_dev, dfu_set_config);
    current_dfu_state = STATE_DFU_IDLE;
    current_dfu_status = DFU_STATUS_OK;
    if (on_state_change) {
        on_state_change(current_dfu_state);
    }
}
