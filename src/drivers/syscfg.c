#include "syscfg.h"
#include "common/common.h"
#include "rcc.h"
#include "stm32f411xe.h"
#include <stdint.h>

void syscfg_map_exti_line(gpio_pin_t gpio_pin) {
  uint8_t pin_num = gpio_pin.pin;
  uint8_t port_num = gpio_pin.port;
  uint8_t index = pin_num / 4;
  uint8_t shift = (pin_num % 4) * 4;

  SYSCFG->EXTICR[index] &= ~(0xF << shift);
  SYSCFG->EXTICR[index] |= (port_num << shift);
}
