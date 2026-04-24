#include "rcc.h"
#include "common/common.h"

void rcc_enable_gpio(gpio_pin_t gpio_pin) {
  uint8_t port_num = get_port_num(gpio_pin);
  RCC->AHB1ENR |= BIT_OFFSET(port_num);
}

void rcc_enable_syscfg(void) { RCC->APB2ENR |= BIT_OFFSET(14); }
