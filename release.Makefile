## Copyright (c) 2017, Devan Lai
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

# This Makefile builds all official targets and places the firmware
# bin files in the build/ directory.
#
# For normal development, use the Makefile in the src/ directory.

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q              := @
NULL           := 2>/dev/null
MAKE           := $(MAKE) --no-print-directory
endif
export V

BUILD_DIR      ?= ./build

all: dapboot-bluepill.bin \
     dapboot-maplemini.bin \
     dapboot-stlink.bin

clean:
	$(Q)$(RM) $(BUILD_DIR)/*.bin
	$(Q)$(MAKE) -C src/ clean

.PHONY = all clean

$(BUILD_DIR):
	$(Q)mkdir -p $(BUILD_DIR)

dapboot-bluepill.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILL -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILL -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-stlink.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=STLINK -C src/ clean
	$(Q)$(MAKE) TARGET=STLINK -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-maplemini.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=MAPLEMINI -C src/ clean
	$(Q)$(MAKE) TARGET=MAPLEMINI -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)
