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

#include <string.h>
#include <libopencm3/cm3/vector.h>

#include "dapboot.h"
#include "target.h"
#include "usb_conf.h"
#include "dfu.h"
#include "webusb.h"
#include "winusb.h"
#include "config.h"

static enum dfu_state current_dfu_state;

static inline void __set_MSP(uint32_t topOfMainStack) {
    asm("msr msp, %0" : : "r" (topOfMainStack));
}

extern volatile const vector_table_t vector_table;

bool validate_application(void) {
    if (((uint32_t)(vector_table.reserved_x001c[0]) & 0x2FFE0000) == 0x20000000) {
        return true;
    }
    return false;
}

static void jump_to_application(void) __attribute__ ((noreturn));

static void jump_to_application(void) {
    /* Initialize the application's stack pointer */
    __set_MSP((uint32_t)(vector_table.reserved_x001c[0]));

    /* Jump to the application entry point */
    vector_table.reserved_x001c[1]();

    while (1);
}

static const char* https_urls[] = {
    "devanlai.github.io/webdfu/dfu-util/",
    "localhost:8000"
};

static void state_change(enum dfu_state new_state) {
    current_dfu_state = new_state;
}

int main(void) {
    /* Setup clocks */
    target_clock_setup();

    /* Initialize GPIO/LEDs if needed */
    target_gpio_setup();

    /* Setup USB */
    {
        char serial[USB_SERIAL_NUM_LENGTH+1];
        serial[0] = '\0';
        target_get_serial_number(serial, USB_SERIAL_NUM_LENGTH);
        usb_set_serial_number(serial);
    }

    usbd_device* usbd_dev = usb_setup();
    dfu_setup(usbd_dev, &target_manifest_app, state_change, NULL);
    webusb_setup(usbd_dev,
                 https_urls, sizeof(https_urls)/sizeof(https_urls[0]));
    winusb_setup(usbd_dev);
        
    if (target_get_force_bootloader() || !validate_application()) {
        while (1) {
            usbd_poll(usbd_dev);
        }
    } else {
#if !HAVE_BUTTON
        for (int dfu_wait = 2000000; dfu_wait > 0; dfu_wait--) {
            usbd_poll(usbd_dev);
	    if (current_dfu_state != STATE_DFU_IDLE)
                dfu_wait = 2000000;
        }
#endif
	target_usb_init();
        jump_to_application();
    }
    
    return 0;
}
