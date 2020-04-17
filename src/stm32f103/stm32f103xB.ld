/*
 * This file is part of the dapboot project.
 *
 * Copyright (C) 2016 Devan Lai <devan.lai@gmail.com>
 * Copyright (C) 2015 Karl Palsson <karlp@tweak.net.au>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Linker script for STM32F103xB, 128k flash, 20k RAM. */

/* Define memory regions. */
/* 6.75k for the bootloader at the end, with a copy of the vector table at
 * the beginning to bootstrap the bootloader
 */
MEMORY
{
	bootstrap (rx) : ORIGIN = 0x08000000, LENGTH = 1K
	rom (rx) : ORIGIN = 0x0801e400, LENGTH = 0x1C00
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE stm32f103/common.ld
