##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

ifeq ($(ARCH),STM32F0)
	LIBNAME		= opencm3_stm32f0
	DEFS		+= -DSTM32F0
	FP_FLAGS	?= -msoft-float
	ARCH_FLAGS	= -mthumb -mcpu=cortex-m0 $(FP_FLAGS)
	OOCD_BOARD	?= target/stm32f0x.cfg
endif
ifeq ($(ARCH),STM32F1)
	LIBNAME		= opencm3_stm32f1
	DEFS		+= -DSTM32F1
	FP_FLAGS	?= -msoft-float
	ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
	OOCD_BOARD	?= target/stm32f1x.cfg
endif

LIBNAME		?= opencm3_stm32f1
DEFS		?= -DSTM32F1
FP_FLAGS    ?= -msoft-float
ARCH_FLAGS	?= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

################################################################################
# OpenOCD specific variables

OOCD		?= openocd
OOCD_INTERFACE	?= interface/cmsis-dap.cfg
OOCD_BOARD	?= target/stm32f1x.cfg

include ../libopencm3.rules.mk
