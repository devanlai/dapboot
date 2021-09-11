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

#ifndef DFU_H_INCLUDED
#define DFU_H_INCLUDED

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dfu.h>

// For WebUSB compatibility
#ifndef DFU_WILL_DETACH
#define DFU_WILL_DETACH 1
#endif

extern const struct usb_dfu_descriptor dfu_function;

typedef bool (*ManifestationCallback)(void);
typedef void (*StateChangeCallback)(enum dfu_state);
typedef void (*StatusChangeCallback)(enum dfu_status);

extern void dfu_setup(usbd_device* usbd_dev,
                      ManifestationCallback on_manifest_request,
                      StateChangeCallback on_state_change,
                      StatusChangeCallback on_status_change);

#endif
