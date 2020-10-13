/*
 * 2020 - Karl Palsson <karlp@tweak.net.au>
 * Considered to be released into the public domain, or where not available
 * under your choice of the following spdx identifiers:
 * MIT, ISC, Apache-2.0, BSD-1-Clause, BSD-2-Clause, BSD-3-Clause,
 * CC-BY-4.0, GPL-2.0-or-later, LGPL-2.0-or-later
 * Pick whatever makes your integration life easier
 *
 * Linker script for STM32L1 parts. Bootloader only for 8k, and all parts
 * have at least 10k RAM, (-A parts have 32K RAM, but 10 is enough)
 */

/* Define memory regions. */
MEMORY
{
	vectors (rx) : ORIGIN = 0x08000000, LENGTH = 0x150
	rom (rx)  : ORIGIN = 0x08000150, LENGTH = 0x1EB0
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 10K
}

/* Include the common ld script. */
INCLUDE stm32l1/stm32l1-base.ld
