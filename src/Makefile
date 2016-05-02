## Copyright (c) 2016, Devan Lai
##
## Permission to use, copy, modify, and/or distribute this software
## for any purpose with or without fee is hereby granted, provided
## that the above copyright notice and this permission notice
## appear in all copies.
##
## THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
## WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
## WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
## AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
## CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
## LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
## NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
## CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-include local.mk

BINARY = dapboot
OPENCM3_DIR = ../libopencm3

TARGET ?= STM32F103
include targets.mk

DFU_UTIL = dfu-util
BOOT_VID_PID := 1209:0001

SRCS := $(wildcard *.c)
SRCS += $(wildcard $(TARGET_DIR)/*.c)

OBJS += $(SRCS:.c=.o)
DEPS  = $(SRCS:.c=.d)

.DEFAULT_GOAL := $(BINARY).bin

%.dfuse-flash: %.bin
	$(DFU_UTIL) -d $(BOOT_VID_PID) -a 0 -s 0x08000000:leave -D $(<)

dfuse-flash: $(BINARY).dfuse-flash

clean::
	@rm -f $(OBJS)
	@rm -f $(DEPS)

include libopencm3.target.mk

size: $(OBJS) $(BINARY).elf
	@$(PREFIX)-size $(OBJS) $(BINARY).elf

debug: $(BINARY).elf
	-$(GDB) --tui --eval "target remote | openocd -f $(OOCD_INTERFACE) -f $(OOCD_BOARD) -f debug.cfg" $(BINARY).elf

.PHONY += debug size dfuse-flash

OBJS := $(sort $(OBJS))

# Add the base directory to the header search path
CPPFLAGS += -I.

# Add target config directory to the header search path
CPPFLAGS += -I$(TARGET_DIR)/
