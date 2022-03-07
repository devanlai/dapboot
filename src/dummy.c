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

/* Default dummy implementations for optional target functions */

#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/cm3/scb.h>

void target_get_serial_number(char* dest, size_t max_chars) __attribute__((weak));
void target_log(const char* str) __attribute__((weak));
void target_pre_main(void) __attribute__((weak));
void target_pre_detach(bool manifested) __attribute__((weak));
void target_post_setup(void) __attribute__((weak));
size_t target_get_timeout(void) __attribute__((weak));

void target_get_serial_number(char* dest, size_t max_chars) {
    (void)max_chars;
    if (dest) {
        dest[0] = '\0';
    }
}

void target_log(const char* str) {
    (void)str;
}

void target_pre_main(void)
{

}

void target_post_setup(void)
{
	/* This runs just before starting to listen to USB */
}


void target_pre_detach(bool manifested) {
    /* This runs just before executing a reboot in response to a USB bus reset
       or a detach request.
       If new firmware was successfully downloaded, manifested is set to true.
       This can be used to set flags or blink LEDs before rebooting.
     */
    (void)manifested;
}

size_t target_get_timeout(void)
{
	return 100;
}
