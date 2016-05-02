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
#include "config.h"

static inline void __set_MSP(uint32_t topOfMainStack) {
    asm("msr msp, %0" : : "r" (topOfMainStack));
}

bool validate_application(void) {
    if ((*(volatile uint32_t *)APP_BASE_ADDRESS & 0x2FFE0000) == 0x20000000) {
        return true;
    }
    return false;
}

static void jump_to_application(void) __attribute__ ((noreturn));

static void jump_to_application(void) {
    vector_table_t* app_vector_table = (vector_table_t*)APP_BASE_ADDRESS;
    
    /* Use the application's vector table */
    target_relocate_vector_table();

    /* Initialize the application's stack pointer */
    __set_MSP((uint32_t)(app_vector_table->initial_sp_value));

    /* Jump to the application entry point */
    app_vector_table->reset();
    
    while (1);
}

static void log_state_change(enum dfu_state state) {
    const char* str = "";
    switch (state) {
        case STATE_APP_IDLE:                str = "appIDLE"; break;
        case STATE_APP_DETACH:              str = "appDETACH"; break;
        case STATE_DFU_IDLE:                str = "dfuIDLE"; break;
        case STATE_DFU_DNLOAD_SYNC:         str = "dfuDNLOAD-SYNC"; break;
        case STATE_DFU_DNBUSY:              str = "dfuDNBUSY"; break;
        case STATE_DFU_DNLOAD_IDLE:         str = "dfuDNLOAD-IDLE"; break;
        case STATE_DFU_MANIFEST_SYNC:       str = "dfuMANIFEST-SYNC"; break;
        case STATE_DFU_MANIFEST:            str = "dfuMANIFEST"; break;
        case STATE_DFU_MANIFEST_WAIT_RESET: str = "dfuMANIFEST-WAIT-RESET"; break;
        case STATE_DFU_UPLOAD_IDLE:         str = "dfuUPLOAD-IDLE"; break;
        case STATE_DFU_ERROR:               str = "dfuERROR"; break;
        default:                            str = "UNKNOWN"; break;
    }

    target_log("state=");
    target_log(str);
    target_log("\r\n");
}

static void log_status_change(enum dfu_status status) {
    const char* str = "";
    switch (status) {
        case DFU_STATUS_OK:                 str = "OK"; break;
        case DFU_STATUS_ERR_TARGET:         str = "errTARGET"; break;
        case DFU_STATUS_ERR_FILE:           str = "errFILE"; break;
        case DFU_STATUS_ERR_WRITE:          str = "errWRITE"; break;
        case DFU_STATUS_ERR_ERASE:          str = "errERASE"; break;
        case DFU_STATUS_ERR_CHECK_ERASED:   str = "errCHECK_ERASED"; break;
        case DFU_STATUS_ERR_PROG:           str = "errPROG"; break;
        case DFU_STATUS_ERR_VERIFY:         str = "errVERIFY"; break;
        case DFU_STATUS_ERR_ADDRESS:        str = "errADDRESS"; break;
        case DFU_STATUS_ERR_NOTDONE:        str = "errNOTDONE"; break;
        case DFU_STATUS_ERR_FIRMWARE:       str = "errFIRMWARE"; break;
        case DFU_STATUS_ERR_VENDOR:         str = "errVENDOR"; break;
        case DFU_STATUS_ERR_USBR:           str = "errUSBR"; break;
        case DFU_STATUS_ERR_POR:            str = "errPOR"; break;
        case DFU_STATUS_ERR_UNKNOWN:        str = "errUNKNOWN"; break;
        case DFU_STATUS_ERR_STALLEDPKT:     str = "errSTALLEDPKT"; break;
        default:                            str = "UNKNOWN"; break;
    }

    target_log("status=");
    target_log(str);
    target_log("\r\n");
}

int main(void) {
    /* Setup clocks */
    target_clock_setup();

    /* Initialize GPIO/LEDs if needed */
    target_gpio_setup();

    target_log("Hello World\r\n");
    
    if (target_get_force_bootloader() || !validate_application()) {
        /* Setup USB */
        {
            char serial[USB_SERIAL_NUM_LENGTH+1];
            serial[0] = '\0';
            target_get_serial_number(serial, USB_SERIAL_NUM_LENGTH);
            usb_set_serial_number(serial);
        }

        usbd_device* usbd_dev = usb_setup();
        dfu_setup(usbd_dev, &target_manifest_app, &log_state_change, &log_status_change);

        while (1) {
            usbd_poll(usbd_dev);
        }
    } else {
        jump_to_application();
    }
    
    return 0;
}
