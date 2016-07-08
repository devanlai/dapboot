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

#ifndef WINUSB_DEFS_H_INCLUDED
#define WINUSB_DEFS_H_INCLUDED

#include <stdint.h>

/* Microsoft OS 1.0 descriptors */

/* Extended Compat ID OS Feature Descriptor Specification */
#define WINUSB_REQ_GET_COMPATIBLE_ID_FEATURE_DESCRIPTOR 0x04
#define WINUSB_REQ_GET_EXTENDED_PROPERTIES_OS_FEATURE_DESCRIPTOR 0x05

/* Table 2. Function Section */
struct winusb_compatible_id_function_section {
    uint8_t  bInterfaceNumber;
    uint8_t  reserved0[1];
    const char compatibleId[8];
    const char subCompatibleId[8];
    uint8_t  reserved1[6];
} __attribute__((packed));

#define WINUSB_COMPATIBLE_ID_FUNCTION_SECTION_SIZE 24

/* Table 1. Header Section */
struct winusb_compatible_id_descriptor {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint8_t  bNumSections;
    uint8_t  reserved[7];
    struct winusb_compatible_id_function_section functions[];
} __attribute__((packed));

#define WINUSB_COMPATIBLE_ID_HEADER_SIZE 16

#endif
