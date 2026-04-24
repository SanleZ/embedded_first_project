#include "exti.h"
#include "common/common.h"
#include <stdint.h>

void exti_configure_line(gpio_pin_t pin, exti_trigger_t trigger) {
  uint8_t pin_num = get_pin_num(pin);
  EXTI->RTSR &= ~BIT_OFFSET(pin_num);
  EXTI->FTSR &= ~BIT_OFFSET(pin_num);

  if (trigger == EXTI_TRIGGER_RISING) {
    EXTI->RTSR |= BIT_OFFSET(pin_num);
  } else if (trigger == EXTI_TRIGGER_FALLING) {
    EXTI->FTSR |= BIT_OFFSET(pin_num);
  } else {
    EXTI->RTSR |= BIT_OFFSET(pin_num);
    EXTI->FTSR |= BIT_OFFSET(pin_num);
  }
}

void exti_enable_line(gpio_pin_t pin) {
  uint8_t pin_num = get_pin_num(pin);
  EXTI->IMR |= BIT_OFFSET(pin_num);
}

void exti_clear_pending(gpio_pin_t pin) {
  uint8_t pin_num = get_pin_num(pin);
  EXTI->PR = BIT_OFFSET(pin_num);
}
