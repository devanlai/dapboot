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
     dapboot-stlink.bin \
     dapboot-olimexstm32h103.bin \
     dapboot-bluepillplusstm32.bin \
     dapboot-bttskrminie3v2.bin \
     dapboot-bluepill-high.bin \
     dapboot-maplemini-high.bin \
     dapboot-stlink-high.bin \
     dapboot-olimexstm32h103-high.bin \
     dapboot-bluepillplusstm32-high.bin \
     dapboot-bluepill-high-128.bin \
     dapboot-maplemini-high-128.bin \
     dapboot-stlink-high-128.bin \
     dapboot-olimexstm32h103-high-128.bin \
     dapboot-bluepillplusstm32-high-128.bin \
     dapboot-bttskrminie3v2-high-256.bin

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

dapboot-olimexstm32h103.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103 -C src/ clean
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bluepillplusstm32.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32 -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bttskrminie3v2.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BTTSKRMINIE3V2 -C src/ clean
	$(Q)$(MAKE) TARGET=BTTSKRMINIE3V2 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bluepill-high.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILL_HIGH -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILL_HIGH -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-stlink-high.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=STLINK_HIGH -C src/ clean
	$(Q)$(MAKE) TARGET=STLINK_HIGH -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-maplemini-high.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=MAPLEMINI_HIGH -C src/ clean
	$(Q)$(MAKE) TARGET=MAPLEMINI_HIGH -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-olimexstm32h103-high.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103_HIGH -C src/ clean
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103_HIGH -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bluepillplusstm32-high.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32_HIGH -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32_HIGH -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bluepill-high-128.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILL_HIGH_128 -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILL_HIGH_128 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-stlink-high-128.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=STLINK_HIGH_128 -C src/ clean
	$(Q)$(MAKE) TARGET=STLINK_HIGH_128 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-maplemini-high-128.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=MAPLEMINI_HIGH_128 -C src/ clean
	$(Q)$(MAKE) TARGET=MAPLEMINI_HIGH_128 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-olimexstm32h103-high-128.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103_HIGH_128 -C src/ clean
	$(Q)$(MAKE) TARGET=OLIMEXSTM32H103_HIGH_128 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bluepillplusstm32-high-128.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32_HIGH_128 -C src/ clean
	$(Q)$(MAKE) TARGET=BLUEPILLPLUSSTM32_HIGH_128 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)

dapboot-bttskrminie3v2-high-256.bin: | $(BUILD_DIR)
	@printf "  BUILD $(@)\n"
	$(Q)$(MAKE) TARGET=BTTSKRMINIE3V2_HIGH_256 -C src/ clean
	$(Q)$(MAKE) TARGET=BTTSKRMINIE3V2_HIGH_256 -C src/
	$(Q)cp src/dapboot.bin $(BUILD_DIR)/$(@)
