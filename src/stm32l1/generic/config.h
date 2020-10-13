/*
 * 2020 - Karl Palsson <karlp@tweak.net.au>
 * Considered to be released into the public domain, or where not available
 * under your choice of the following spdx identifiers:
 * MIT, ISC, Apache-2.0, BSD-1-Clause, BSD-2-Clause, BSD-3-Clause,
 * CC-BY-4.0, GPL-2.0-or-later, LGPL-2.0-or-later
 * Pick whatever makes your integration life easier
 *
 * defauly config.h settings
 */

#pragma once

#define APP_BASE_ADDRESS (0x08000000 + BOOTLOADER_OFFSET)
#define FLASH_PAGE_SIZE  128
#define DFU_UPLOAD_AVAILABLE 1
#define DFU_DOWNLOAD_AVAILABLE 1
#define TARGET_DFU_WTRANSFERSIZE 128

#define HAVE_LED 0
#define HAVE_BUTTON 0
#define HAVE_USB_PULLUP_CONTROL 0
