// #include "core/systick.h"
// #include "drivers/exti.h"
// #include "drivers/gpio.h"
// #include <stdint.h>

// volatile uint8_t button_pressed_flag = 0;
//
// void EXTI0_IRQHandler(void) {
//   if (EXTI->PR & (1 << 0)) {
//     EXTI->PR = (1 << 0);
//
//     static uint32_t last_time = 0;
//
//     if ((ticks - last_time) > 200) {
//       last_time = ticks;
//       gpio_t *gpio = GPIO(0);
//       if (!(gpio->IDR & (1 << 0))) {
//         button_pressed_flag = 1;
//       }
//     }
//   }
// }
#include "drivers/exti.h"

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
