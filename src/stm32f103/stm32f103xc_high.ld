/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2015 Karl Palsson <karlp@tweak.net.au>
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

/* Linker script for STM32F103xC, 256k flash, 48k RAM. */

/* Define memory regions. */
MEMORY
{
	vectors (rx) : ORIGIN = 0x08000000, LENGTH = 0x150
	rom (rx)  : ORIGIN = 0x0803E400, LENGTH = 0x1C00
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 48K
}

/* Include the common ld script. */
INCLUDE stm32f103/stm32f1.ld
