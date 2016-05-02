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

/* STM32F103-specific target functions */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/cm3/scb.h>

#include "target.h"
#include "config.h"
#include "backup.h"
#define LED_OPEN_DRAIN 0

static const uint32_t CMD_BOOT = 0x544F4F42UL;

void target_clock_setup(void) {
    /* Set system clock to 72 MHz */
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

void target_gpio_setup(void) {
    /*
      LED0 on PA9
    */

    /* Enable GPIOA clock */
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Setup LEDs */
    const uint8_t mode = GPIO_MODE_OUTPUT_10_MHZ;
    const uint8_t conf = (LED_OPEN_DRAIN ? GPIO_CNF_OUTPUT_OPENDRAIN
                                         : GPIO_CNF_OUTPUT_PUSHPULL);
    gpio_set_mode(GPIOA, mode, conf, GPIO9);
}

const usbd_driver* target_usb_init(void) {
    rcc_periph_reset_pulse(RST_USB);

    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    int i;
    for (i = 0; i < 800000; i++)
        __asm__("nop");
    
    return &st_usbfs_v1_usb_driver;
}

bool target_get_force_bootloader(void) {
    bool force = false;
    /* Check the RTC backup register */
    uint32_t cmd = backup_read(BKP0);
    if (cmd == CMD_BOOT) {
        force = true;
    }
    
    /* Clear the RTC backup register */
    backup_write(BKP0, 0);
    
    return force;
}

void target_get_serial_number(char* dest, size_t max_chars) {
    desig_get_unique_id_as_string(dest, max_chars+1);
}

size_t target_get_max_firmware_size(void) {
    uint8_t* flash_end = (void*)(FLASH_BASE + DESIG_FLASH_SIZE*1024);
    uint8_t* flash_start = (void*)(APP_BASE_ADDRESS);

    return (size_t)(flash_end - flash_start);
}

void target_relocate_vector_table(void) {
    SCB_VTOR = APP_BASE_ADDRESS & 0xFFFF;
}

void target_flash_unlock(void) {
    flash_unlock();
}

void target_flash_lock(void) {
    flash_lock();
}

static inline uint16_t* get_flash_page_address(uint16_t* dest) {
    return (uint16_t*)(((uint32_t)dest / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE);
}
static uint16_t* erase_start;
static uint16_t* erase_end;
bool target_flash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count, bool verify) {
    bool verified = true;

    /* Remember the bounds of erased data in the current page */

    if (verify) {
        while (half_word_count > 0) {
            if (dest >= erase_end || dest < erase_start) {
                erase_start = get_flash_page_address(dest);
                erase_end = erase_start + (FLASH_PAGE_SIZE)/sizeof(uint16_t);
                flash_erase_page((uint32_t)erase_start);
            }
            flash_program_half_word((uint32_t)dest, *data);
            erase_start = dest + 1;
            if (*dest != *data) {
                verified = false;
                break;
            }
            dest++;
            data++;
            half_word_count--;
        }
    } else {
        while (half_word_count > 0) {
            if (dest >= erase_end || dest < erase_start) {
                erase_start = get_flash_page_address(dest);
                erase_end = erase_start + (FLASH_PAGE_SIZE)/sizeof(uint16_t);
                flash_erase_page((uint32_t)erase_start);
            }
            flash_program_half_word((uint32_t)dest, *data);
            erase_start = dest + 1;
            dest++;
            data++;
            half_word_count--;
        }
    }

    return verified;
}
