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
#include "config.h"
#include "usb_conf.h"
#include "dfu.h"
#include "webusb.h"
#include "winusb.h"

static inline void __set_MSP(uint32_t topOfMainStack) {
    asm("msr msp, %0" : : "r" (topOfMainStack));
}

bool validate_application(void) {
    return ((uint32_t)(APP_INITIAL_STACK) & 0x2FFE0000) == 0x20000000;
}

static void jump_to_application(void) __attribute__ ((noreturn));

static void jump_to_application(void) {

    /* Use the application's vector table */
    if (APP_RELOCATE_VECTORS)
        target_relocate_vector_table();

    /* Do any necessary early setup for the application */
    target_pre_main();

    /* Initialize the application's stack pointer */
    __set_MSP((uint32_t)(APP_INITIAL_STACK));

    /* Jump to the application entry point */
    APP_ENTRY_POINT();
    
    while (1);
}

int main(void) {
    /* Setup clocks */
    target_clock_setup();

    /* Initialize GPIO/LEDs if needed */
    target_gpio_setup();

    if (target_get_force_bootloader() || !validate_application()) {
        /* Setup USB */
        {
            char serial[USB_SERIAL_NUM_LENGTH+1];
            serial[0] = '\0';
            target_get_serial_number(serial, USB_SERIAL_NUM_LENGTH);
            usb_set_serial_number(serial);
        }

        usbd_device* usbd_dev = usb_setup();
        dfu_setup(usbd_dev, validate_application, NULL, NULL);
        webusb_setup(usbd_dev);
        winusb_setup(usbd_dev);
        target_post_setup();

        while (1) {
            usbd_poll(usbd_dev);
        }
    } else {
        jump_to_application();
    }
    
    return 0;
}
