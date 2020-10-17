##
## This file is derived from the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
## Copyright (C) 2016 Devan Lai
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

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q              := @
NULL           := 2>/dev/null
endif

####################################################################
# Target Architecture flags
ifeq ($(ARCH),STM32F0)
	LIBNAME     = opencm3_stm32f0
	DEFS       += -DSTM32F0
	FP_FLAGS   ?= -msoft-float
	ARCH_FLAGS  = -mthumb -mcpu=cortex-m0 $(FP_FLAGS)
	OOCD_BOARD ?= target/stm32f0x.cfg
	OPENCM3_TARGET = "stm32/f0"
endif
ifeq ($(ARCH),STM32F1)
	LIBNAME     = opencm3_stm32f1
	DEFS       += -DSTM32F1
	FP_FLAGS   ?= -msoft-float
	ARCH_FLAGS  = -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
	OOCD_BOARD ?= target/stm32f1x.cfg
	OPENCM3_TARGET = "stm32/f1"
endif
ifeq ($(ARCH),STM32L1)
	LIBNAME     = opencm3_stm32l1
	DEFS       += -DSTM32L1
	FP_FLAGS   ?= -msoft-float
	ARCH_FLAGS  = -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
	OOCD_BOARD ?= target/stm32l1.cfg
	OPENCM3_TARGET = "stm32/l1"
endif

LIBNAME        ?= opencm3_stm32f0
DEFS           ?= -DSTM32F0
FP_FLAGS       ?= -msoft-float
ARCH_FLAGS     ?= -mthumb -mcpu=cortex-m0 $(FP_FLAGS)
OPENCM3_TARGET ?= "stm32/f0"

####################################################################
# Semihosting support
SEMIHOSTING    ?= 0

ifeq ($(SEMIHOSTING),1)
	LDFLAGS    += --specs=rdimon.specs
	LDLIBS     += -lrdimon
	DEFS       += -DSEMIHOSTING=1
else
	DEFS       += -DSEMIHOSTING=0
endif

####################################################################
# OpenOCD specific variables

OOCD           ?= openocd
OOCD_INTERFACE ?= interface/cmsis-dap.cfg
OOCD_BOARD     ?= target/stm32f1x.cfg

####################################################################
# Executables

PREFIX         ?= arm-none-eabi-

CC             := $(PREFIX)gcc
CXX            := $(PREFIX)g++
LD             := $(PREFIX)gcc
AR             := $(PREFIX)gcc-ar
AS             := $(PREFIX)as
OBJCOPY        := $(PREFIX)objcopy
OBJDUMP        := $(PREFIX)objdump
GDB            := $(PREFIX)gdb
STFLASH         = $(shell which st-flash)

####################################################################
# Source files

INCLUDE_DIR = $(OPENCM3_DIR)/include
LIB_DIR     = $(OPENCM3_DIR)/lib

####################################################################
# C flags

CFLAGS      += -Os -flto -g -std=gnu11
CFLAGS      += -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS      += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS      += -fno-common -ffunction-sections -fdata-sections

####################################################################
# C++ flags

CXXFLAGS    += -Os -flto -g
CXXFLAGS    += -Wextra -Wshadow -Wredundant-decls  -Weffc++
CXXFLAGS    += -fno-common -ffunction-sections -fdata-sections

####################################################################
# C & C++ preprocessor common flags

CPPFLAGS    += -MD
CPPFLAGS    += -Wall -Wundef
CPPFLAGS    += -I$(INCLUDE_DIR) $(DEFS)

####################################################################
# Linker flags

LDFLAGS    += -flto -Os -g --static -nostartfiles
LDFLAGS    += -specs=nano.specs
LDFLAGS    += -L$(LIB_DIR)
LDFLAGS    += -T$(LDSCRIPT)
LDFLAGS    += -Wl,-Map=$(*).map
LDFLAGS    += -Wl,--gc-sections
ifeq ($(V),99)
LDFLAGS    += -Wl,--print-gc-sections
endif

####################################################################
# Used libraries

LDLIBS     += -l$(LIBNAME)
LDLIBS     += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

####################################################################
####################################################################
####################################################################

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: elf

elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list

images: $(BINARY).images
flash: $(BINARY).flash

$(LDSCRIPT):
    ifeq (,$(wildcard $(LDSCRIPT)))
        $(error Unable to find specified linker script: $(LDSCRIPT))
    endif

$(OPENCM3_DIR)/Makefile:
	$(Q)git submodule update --init $(OPENCM3_DIR)

# If overriding the toolchain, use it when building LOCM3
export PREFIX

$(LIB_DIR)/lib$(LIBNAME).a: $(OPENCM3_DIR)/Makefile
	$(Q)$(MAKE) -C $(OPENCM3_DIR) AR=$(AR) CFLAGS="-flto -g" TARGETS=$(OPENCM3_TARGET)

locm3: $(LIB_DIR)/lib$(LIBNAME).a

%.images: %.bin %.hex %.srec %.list %.map
	@#printf "*** $* images generated ***\n"

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary --gap-fill=0xff $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@#printf "  OBJCOPY $(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@#printf "  OBJDUMP $(*).list\n"
	$(Q)$(OBJDUMP) -S $(*).elf > $(*).list

%.elf %.map: $(OBJS) $(LDSCRIPT) $(LIB_DIR)/lib$(LIBNAME).a
	@#printf "  LD      $(*).elf\n"
	$(Q)$(LD) $(LDFLAGS) $(ARCH_FLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.o: %.c $(LIB_DIR)/lib$(LIBNAME).a
	@#printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).c

%.o: %.cxx $(LIB_DIR)/lib$(LIBNAME).a
	@#printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).cxx

%.o: %.cpp $(LIB_DIR)/lib$(LIBNAME).a
	@#printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).cpp

clean::
	@#printf "  CLEAN\n"
	$(Q)$(RM) *.o *.d *.elf *.bin *.hex *.srec *.list *.map

%.stlink-flash: %.bin
	@printf "  FLASH  $<\n"
	$(Q)$(STFLASH) write $(*).bin 0x08000000

%.flash: %.elf
	@printf "  FLASH   $<\n"
	$(Q)$(OOCD) -f $(OOCD_INTERFACE) \
			-f $(OOCD_BOARD) \
			-c "program $(*).elf verify reset exit" \
			$(NULL)

.PHONY: images clean elf bin hex srec list locm3

-include $(OBJS:.o=.d)
