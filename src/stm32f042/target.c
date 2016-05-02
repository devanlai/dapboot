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

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/crs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/syscfg.h>
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
    /* Set the system clock to 48 MHz */
    rcc_clock_setup_in_hsi48_out_48mhz();

    // Trim from USB sync frame
    crs_autotrim_usb_enable();
    rcc_set_usbclk_source(RCC_HSI48);
}

void target_gpio_setup(void) {
    /*
      Button on PB8
      LED0, 1, 2 on PA0, PA1, PA4
    */
    
    /* Enable GPIOA and GPIOB clocks. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);


    /* Setup LEDs */
    const uint8_t conf = (LED_OPEN_DRAIN ? GPIO_OTYPE_OD
                                         : GPIO_OTYPE_PP);
    
    gpio_set_output_options(GPIOA, conf, GPIO_OSPEED_LOW,
                            GPIO0 | GPIO1 | GPIO4);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                    GPIO0 | GPIO1 | GPIO4);

    /* Set PB8 to an input */
    gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8);
}

const usbd_driver* target_usb_init(void) {
    rcc_periph_reset_pulse(RST_USB);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_SYSCFG_COMP);

    /* Remap PA11 and PA12 for use as USB */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                    GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF2, GPIO11 | GPIO12);
    SYSCFG_CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;
    return &st_usbfs_v2_usb_driver;
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

    /* Force bootloader if button pressed */
    if (gpio_get(GPIOB, GPIO8) != 0) {
        force = true;
    }

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
    /* no-op; the vector table can't be directly relocated */
}

void target_flash_unlock(void) {
    flash_unlock();
}

void target_flash_lock(void) {
    flash_lock();
}

bool target_flash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count, bool verify) {
    bool verified = true;

    if (verify) {
        while (half_word_count > 0) {
            flash_program_half_word((uint32_t)dest, *data);
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
            flash_program_half_word((uint32_t)dest, *data);
            dest++;
            data++;
            half_word_count--;
        }
    }

    return verified;
}
