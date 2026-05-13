#include "drivers/exti.h"
#include <stdint.h>

void EXTI0_IRQHandler(void) { exti_handle_irq(0); }

void EXTI1_IRQHandler(void) { exti_handle_irq(1); }

void EXTI2_IRQHandler(void) { exti_handle_irq(2); }

void EXTI3_IRQHandler(void) { exti_handle_irq(3); }

void EXTI4_IRQHandler(void) { exti_handle_irq(4); }

void EXTI9_5_IRQHandler(void) {
  for (uint8_t line = 5; line <= 9; line++) {
    exti_handle_irq(line);
  }
}

void EXTI15_10_IRQHandler(void) {
  for (uint8_t line = 10; line <= 15; line++) {
    exti_handle_irq(line);
  }
}
