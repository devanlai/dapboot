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

#ifndef DAPBOOT_H_INCLUDED
#define DAPBOOT_H_INCLUDED

extern bool validate_application(void);

#ifdef BOOTLOADER_HIGH
/* Definitions for high memory bootloader */
#define APP_INITIAL_STACK    vector_table.reserved_x001c[0]
#define APP_ENTRY_POINT      vector_table.reserved_x001c[1]
#define APP_RELOCATE_VECTORS 0
#define DFU_PATCH_VECTORS    1
#define BOOTLOADER_OFFSET    0

#else
/* Definitions for low memory (standard) bootloader */
#define APP_INITIAL_STACK ((vector_table_t*)APP_BASE_ADDRESS)->initial_sp_value
#define APP_ENTRY_POINT   ((vector_table_t*)APP_BASE_ADDRESS)->reset
#define APP_RELOCATE_VECTORS 1
#define DFU_PATCH_VECTORS    0
#define BOOTLOADER_OFFSET    0x00002000

#endif

#endif
