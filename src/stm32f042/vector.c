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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/vector.h>

#include "config.h"

static inline vector_table_entry_t get_app_irq(int nvic) {
    volatile vector_table_t* app_vector_table = (volatile vector_table_t*)APP_BASE_ADDRESS;
    return app_vector_table->irq[nvic];
}

#define CORE_IRQ_WRAPPER(IRQ_NAME, ATTR_NAME) \
void IRQ_NAME(void) { \
    volatile vector_table_t* app_vector_table = (volatile vector_table_t*)APP_BASE_ADDRESS; \
    app_vector_table->ATTR_NAME(); \
}

#define IRQ_WRAPPER(IRQ_NAME, NVIC) \
void IRQ_NAME(void); \
void IRQ_NAME(void) { \
    get_app_irq(NVIC)(); \
}

CORE_IRQ_WRAPPER(nmi_handler, nmi)
CORE_IRQ_WRAPPER(hard_fault_handler, hard_fault)
CORE_IRQ_WRAPPER(sv_call_handler, sv_call)
CORE_IRQ_WRAPPER(pend_sv_handler, pend_sv)
CORE_IRQ_WRAPPER(sys_tick_handler, systick)

IRQ_WRAPPER(wwdg_irq, NVIC_WWDG_IRQ)
IRQ_WRAPPER(pvd_irq, NVIC_PVD_IRQ)
IRQ_WRAPPER(rtc_irq, NVIC_RTC_IRQ)
IRQ_WRAPPER(flash_irq, NVIC_FLASH_IRQ)
IRQ_WRAPPER(rcc_irq, NVIC_RCC_IRQ)
IRQ_WRAPPER(exti0_1_irq, NVIC_EXTI0_1_IRQ)
IRQ_WRAPPER(exti2_3_irq, NVIC_EXTI2_3_IRQ)
IRQ_WRAPPER(exti4_15_irq, NVIC_EXTI4_15_IRQ)
IRQ_WRAPPER(tsc_irq, NVIC_TSC_IRQ)
IRQ_WRAPPER(dma1_channel1_irq, NVIC_DMA1_CHANNEL1_IRQ)
IRQ_WRAPPER(dma1_channel2_3_irq, NVIC_DMA1_CHANNEL2_3_IRQ)
IRQ_WRAPPER(dma1_channel4_5_irq, NVIC_DMA1_CHANNEL4_5_IRQ)
IRQ_WRAPPER(adc_comp_irq, NVIC_ADC_COMP_IRQ)
IRQ_WRAPPER(tim1_brk_up_trg_com_irq, NVIC_TIM1_BRK_UP_TRG_COM_IRQ)
IRQ_WRAPPER(tim1_cc_irq, NVIC_TIM1_CC_IRQ)
IRQ_WRAPPER(tim2_irq, NVIC_TIM2_IRQ)
IRQ_WRAPPER(tim3_irq, NVIC_TIM3_IRQ)
IRQ_WRAPPER(tim6_dac_irq, NVIC_TIM6_DAC_IRQ)
IRQ_WRAPPER(tim7_irq, NVIC_TIM7_IRQ)
IRQ_WRAPPER(tim14_irq, NVIC_TIM14_IRQ)
IRQ_WRAPPER(tim15_irq, NVIC_TIM15_IRQ)
IRQ_WRAPPER(tim16_irq, NVIC_TIM16_IRQ)
IRQ_WRAPPER(tim17_irq, NVIC_TIM17_IRQ)
IRQ_WRAPPER(i2c1_irq, NVIC_I2C1_IRQ)
IRQ_WRAPPER(i2c2_irq, NVIC_I2C2_IRQ)
IRQ_WRAPPER(spi1_irq, NVIC_SPI1_IRQ)
IRQ_WRAPPER(spi2_irq, NVIC_SPI2_IRQ)
IRQ_WRAPPER(usart1_irq, NVIC_USART1_IRQ)
IRQ_WRAPPER(usart2_irq, NVIC_USART2_IRQ)
IRQ_WRAPPER(usart3_4_irq, NVIC_USART3_4_IRQ)
IRQ_WRAPPER(cec_can_irq, NVIC_CEC_CAN_IRQ)
IRQ_WRAPPER(usb_irq, NVIC_USB_IRQ)
