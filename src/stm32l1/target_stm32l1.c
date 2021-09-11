/*
 * 2020 - Karl Palsson <karlp@tweak.net.au>
 * Considered to be released into the public domain, or where not available
 * under your choice of the following spdx identifiers:
 * MIT, ISC, Apache-2.0, BSD-1-Clause, BSD-2-Clause, BSD-3-Clause,
 * CC-BY-4.0, GPL-2.0-or-later, LGPL-2.0-or-later
 * Pick whatever makes your integration life easier
 *
 * STM32L1 generic target side portions for dapboot.
 * There's no "standard" l1 board, so you're always going 
 * to want to edit this...
 */

#include <assert.h>

#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/stm32/syscfg.h>

#include "dapboot.h"
#include "target.h"
#include "config.h"
#include "backup.h"

#ifndef REG_BOOT
#define REG_BOOT BKP1
#endif

#ifndef CMD_BOOT
#define CMD_BOOT 0x544F4F42UL
#endif

//#define CMD_FAST_BOOT 0xfa57b007

void target_clock_setup(void) {

//#define MODERN_LOCM3
	/* Clock struct for "any" board with a 16Mhz crystal */
	const struct rcc_clock_scale myclock_16m_hse = {
		.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
		.pll_mul = RCC_CFGR_PLLMUL_MUL6,
		.pll_div = RCC_CFGR_PLLDIV_DIV3,
		.hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_waitstates = 1,
		.ahb_frequency = 32e6,
		.apb1_frequency = 32e6,
		.apb2_frequency = 32e6,
	};

	rcc_clock_setup_pll(&myclock_16m_hse);
}

void target_gpio_setup(void)
{
	/* TODO: if you need buttons or gpios, turn them on here */
#if HAVE_LED
#endif
#if HAVE_BUTTON
#endif
}

const usbd_driver* target_usb_init(void)
{
	rcc_periph_reset_pulse(RST_USB);

	/* Enable built in USB pullup on L1, note, this is out of spec on older revs! */
        rcc_periph_clock_enable(RCC_SYSCFG);
/* Compat for old library */
#ifndef SYSCFG_PMC_USB_PU
#define SYSCFG_PMC_USB_PU                      (1<<0)
#endif
        SYSCFG_PMC |= SYSCFG_PMC_USB_PU;

	return &st_usbfs_v1_usb_driver;
}

/* This implementation will always start in bootloader, unless the app
 * has asked it to skipp straight forwards
 * You may wish to fill in button handling...
 */
bool target_get_force_bootloader(void)
{
	bool enter_bl = false;
	uint32_t cmd = backup_read(REG_BOOT);
	if (cmd == CMD_BOOT) {
		enter_bl = true;
	}
	backup_write(REG_BOOT, 0);

#if HAVE_BUTTON
#warning HAVE_BUTTON not implemented for L1
#endif
	
	return enter_bl;
}

void target_get_serial_number(char* dest, size_t max_chars)
{
	/* TODO, but not available in old locm3...
	assert(max_chars > 6);
	desig_get_unique_id_as_dfu(dest);
	*/
	desig_get_unique_id_as_string(dest, max_chars+1);
}

size_t target_get_max_firmware_size(void)
{
	/* L1, unlike magical fairy land F1, tells it like it is */
	/* mask is working around bug upstream for L1 */
	size_t flash = desig_get_flash_size() & 0xff;
	size_t total = flash * 1024;
#ifdef BOOTLOADER_HIGH
#error bootloader high memory mode not supported on L1
#endif
	return total - BOOTLOADER_OFFSET;
}


#ifndef MODERN_LOCM3
/* These two are proposed upstream, but we need them here now */
static void flash_erase_page(uint32_t page_address)
{
	FLASH_PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG;
	/* L1 requires first word in page, L0 doesn't care, user take care */
	MMIO32(page_address) = 0;
	while ((FLASH_SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	FLASH_PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG);
}

/* Must be run from RAM (per ref manual), and because it's in ram, more
 * than 64MB away from flash address space, must be a long_call.
 * (see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78903 for noinline) */
void flash_program_half_page(uint32_t *dst, const uint32_t *buf);
__attribute__ ((noinline, long_call, section (".ramtext")))
void flash_program_half_page(uint32_t *dst, const uint32_t *buf)
{
        const uint32_t *src = buf;

        /* Enable half page writes to program memory */
        FLASH_PECR |= FLASH_PECR_FPRG | FLASH_PECR_PROG;
        while ((FLASH_SR & FLASH_SR_BSY) == FLASH_SR_BSY);
#ifndef FLASH_HALF_PAGE_SIZE
#define FLASH_HALF_PAGE_SIZE 32 /* For L1, 16 for L0 */
#endif
        for (int i = 0; i < FLASH_HALF_PAGE_SIZE; i++) {
                *dst++ = *src++;
        }
        while ((FLASH_SR & FLASH_SR_BSY) == FLASH_SR_BSY);
        FLASH_PECR &= ~(FLASH_PECR_FPRG | FLASH_PECR_PROG);
}
#endif

static bool target_flash_program_array_nice(uint32_t dest, const uint32_t* data, size_t word_count)
{
	/* whole page was erased, you're doing the full half page */
	(void)word_count;
	assert(word_count <= 32);
	if ((dest & (256-1)) == 0) {
		flash_erase_page(dest);
	}
	cm_disable_interrupts();
	flash_program_half_page((uint32_t *)dest, data);
	cm_enable_interrupts();
	return true;
}

/**
 * The API is (forrealz?!) tied to the F1 16bit halfwords!
 * backconvert, and call a "nicer" api
 */
bool target_flash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count)
{
	return target_flash_program_array_nice((uint32_t)dest, (const uint32_t *)data, half_word_count / 2);
}

/* L1 does shorter transfers, and writes half pages at a time,
 * can/should be polled more frequently */
size_t target_get_timeout(void)
{
	return 5;
}

/* this is all common code below, extract for upstream? */
void target_relocate_vector_table(void) {
    SCB_VTOR = APP_BASE_ADDRESS & 0xFFFF;
}

void target_flash_unlock(void) {
    flash_unlock();
}

void target_flash_lock(void) {
    flash_lock();
}

