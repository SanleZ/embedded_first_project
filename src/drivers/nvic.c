#include "nvic.h"
#include "common/common.h"

void nvic_enable_irq(IRQn_Type irq) {
  /* Each ISER register controls 32 IRQs */
  uint8_t reg_index = irq / 32;
  uint8_t bit_pos = irq % 32;
  // each register == 32 bits == 4 bytes
  volatile uint32_t *iser = (uint32_t *)(NVIC_ISER_BASE + reg_index * 4);
  *iser = BIT_OFFSET(bit_pos);
}

void nvic_disable_irq(IRQn_Type irq) {
  /* Each ICER register controls 32 IRQs */
  uint8_t reg_index = irq / 32;
  uint8_t bit_pos = irq % 32;

  volatile uint32_t *icer = (uint32_t *)(NVIC_ICER_BASE + reg_index * 4);
  // Writing 1 disables the interrupt
  *icer = BIT_OFFSET(bit_pos);
}
