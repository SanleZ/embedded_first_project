#include "drivers/exti.h"
#include <stdint.h>
volatile uint8_t button_pressed_flag = 0;

void EXTI0_IRQHandler(void) {
  if (EXTI->PR & (1 << 0)) {
    EXTI->PR = (1 << 0);
    button_pressed_flag = 1;
  }
}
