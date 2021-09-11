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

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dfu.h>

#include "config.h"
#include "usb_conf.h"
#include "dfu.h"
#include "dfu_defs.h"
#include "target.h"
#include "dapboot.h"

#ifndef TARGET_DFU_WTRANSFERSIZE
#define TARGET_DFU_WTRANSFERSIZE USB_CONTROL_BUF_SIZE
#endif

const struct usb_dfu_descriptor dfu_function = {
    .bLength = sizeof(struct usb_dfu_descriptor),
    .bDescriptorType = DFU_FUNCTIONAL,
    .bmAttributes = ((DFU_DOWNLOAD_AVAILABLE ? USB_DFU_CAN_DOWNLOAD : 0) |
                     (DFU_UPLOAD_AVAILABLE ? USB_DFU_CAN_UPLOAD : 0) |
                     (DFU_WILL_DETACH ? 0 : USB_DFU_MANIFEST_TOLERANT) |
                     (DFU_WILL_DETACH ? USB_DFU_WILL_DETACH : 0)),
    .wDetachTimeout = 255,
    .wTransferSize = TARGET_DFU_WTRANSFERSIZE,
    .bcdDFUVersion = 0x0110,
};

static enum dfu_state current_dfu_state;
static enum dfu_status current_dfu_status;
static size_t current_dfu_offset;

static bool manifestation_complete = false;

static uint8_t dfu_download_buffer[USB_CONTROL_BUF_SIZE];
static size_t dfu_download_size;

/* User callbacks */
static ManifestationCallback dfu_manifest_request_callback = NULL;
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

static void dfu_on_detach_complete(usbd_device* usbd_dev, struct usb_setup_data* req) {
    (void)usbd_dev;
    (void)req;

    /* Run the target-specific pre-detach hook before resetting */
    target_pre_detach(manifestation_complete);

    /* Reset and maybe launch the application */
    scb_reset_system();
}

static void dfu_on_download_request(usbd_device* usbd_dev, struct usb_setup_data* req) {
    (void)usbd_dev;
    (void)req;

    if (DFU_PATCH_VECTORS && current_dfu_offset == 0) {
        if (dfu_download_size < offsetof(vector_table_t, reserved_x001c[1])) {
            /* Can't handle splitting the vector table right now */
            dfu_set_status(DFU_STATUS_ERR_VENDOR);
        } else {
            vector_table_t* app_vector_table = (vector_table_t*)dfu_download_buffer;
            /* Stash the application's initial stack value and reset
               pointer in unused vector table entries */
            app_vector_table->reserved_x001c[0] = (vector_table_entry_t)(app_vector_table->initial_sp_value);
            app_vector_table->reserved_x001c[1] = app_vector_table->reset;
            /* Overwrite the stack and reset pointer to run the
               bootloader instead */
            app_vector_table->initial_sp_value = &_stack;
            app_vector_table->reset = reset_handler;
        }
    }

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
        /* The manifestation callback returns a boolean indicating if it succeeded */
        manifestation_complete = dfu_manifest_request_callback();
    } else {
        /* Assume manifestation success */
        manifestation_complete = true;
    }

    if (manifestation_complete) {
        dfu_set_state(STATE_DFU_MANIFEST_SYNC);
    } else {
        dfu_set_status(DFU_STATUS_ERR_FIRMWARE);
        return; /* Avoid resetting on error */
    }

#if DFU_WILL_DETACH
    /* DFU_WILL_DETACH being enabled equates to transitioning to the dfuMANIFEST-WAIT-RESET state,
     * which combined with bitWillDetach being set with DFU_WILL_DETACH means that the device should
     * generate a detach-attach sequence and enter the application, i.e. reset itself, here */
    dfu_on_detach_complete(NULL, NULL);
#endif
}

static enum usbd_request_return_codes
dfu_control_class_request(usbd_device *usbd_dev,
                          struct usb_setup_data *req,
                          uint8_t **buf, uint16_t *len,
                          usbd_control_complete_callback* complete) {
    if (req->wIndex != INTF_DFU) {
        return USBD_REQ_NEXT_CALLBACK;
    }

    enum usbd_request_return_codes status = USBD_REQ_HANDLED;
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
                    bwPollTimeout = target_get_timeout();
                    *complete = &dfu_on_download_request;
                    break;
                }
                case STATE_DFU_MANIFEST_SYNC: {
                    /* According to the DFU spec the dfuMANIFEST-SYNC state is entered twice,
                     * once after the download completes, and again after manifestation if
                     * the device is manifestation tolerant (DFU_WILL_DETACH == 0) */
                    if (manifestation_complete) {
                        /* Only enter idle state after manifestation has completed successfully */
                        dfu_set_state(STATE_DFU_IDLE);
                    } else {
                        /* Perform manifestation after download as described in the
                         * spec regardless of if DFU_WILL_DETACH is enabled or not */
                        dfu_set_state(STATE_DFU_MANIFEST);
                        *complete = &dfu_on_manifest_request;
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
                        /* Reset manifestation progress on new download */
                        manifestation_complete = false;
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
                    __attribute__ ((fallthrough));
                }
                case STATE_DFU_UPLOAD_IDLE: {
                    uint16_t len_to_copy = req->wLength;
                    size_t max_firmware_size = target_get_max_firmware_size();
                    if (current_dfu_offset + req->wLength > max_firmware_size) {
                        len_to_copy = max_firmware_size - current_dfu_offset;
                        dfu_set_state(STATE_DFU_IDLE);
                    } else {
                        dfu_set_state(STATE_DFU_UPLOAD_IDLE);
                    }
                    *len = len_to_copy;
                    if (DFU_PATCH_VECTORS && current_dfu_offset < sizeof(vector_table_t)) {
                        /* Copy the flash memory to the download buffer, to
                           undo the vector modifications. */
                        memcpy(dfu_download_buffer, (const void*)(APP_BASE_ADDRESS),
                               sizeof(dfu_download_buffer));
                        vector_table_t* app_vector_table = (vector_table_t*)dfu_download_buffer;
                        /* Put the original stack pointer and reset vectors
                           back */
                        app_vector_table->initial_sp_value = (unsigned int*)(app_vector_table->reserved_x001c[0]);
                        app_vector_table->reset = app_vector_table->reserved_x001c[1];
                        app_vector_table->reserved_x001c[0] = 0;
                        app_vector_table->reserved_x001c[1] = 0;
                        /* Return the correct pointer */
                        *buf = dfu_download_buffer + current_dfu_offset;
                    } else {
                        *buf = (uint8_t*)(APP_BASE_ADDRESS + current_dfu_offset);
                    }
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

        case DFU_DETACH: {
            *complete = &dfu_on_detach_complete;
            status = USBD_REQ_HANDLED;
            break;
        }

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

/* Track dfu enumeration status to distinguish between the first USB
   reset after bootup versus a subsequent USB reset and re-enumeration */
static bool dfu_enumerated = false;

static void dfu_set_config(usbd_device* usbd_dev, uint16_t wValue) {
    (void)wValue;

    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        dfu_control_class_request);

    dfu_enumerated = true;
}

static void dfu_on_usb_reset(void) {
    /* Ignore all USB resets until the DFU control callback has been registered, since
     * reset callback will fire once as the USB connection is established. Without this
     * the target enters a reset loop when trying to enter the bootloader. */
    if (!dfu_enumerated) {
        return;
    }

    /* Perform a DFU detach (which resets the target), this enables issuing a USB bus
     * reset as an alternative means to submitting a DFU_DETACH command post-download. */
    dfu_on_detach_complete(NULL, NULL);
}

void dfu_setup(usbd_device* usbd_dev,
               ManifestationCallback on_manifest_request,
               StateChangeCallback on_state_change,
               StatusChangeCallback on_status_change) {
    dfu_manifest_request_callback = on_manifest_request;
    dfu_state_change_callback = on_state_change;
    dfu_status_change_callback = on_status_change;

    usbd_register_reset_callback(usbd_dev, dfu_on_usb_reset);
    usbd_register_set_config_callback(usbd_dev, dfu_set_config);
    current_dfu_state = STATE_DFU_IDLE;
    current_dfu_status = DFU_STATUS_OK;
    if (on_state_change) {
        on_state_change(current_dfu_state);
    }
}
