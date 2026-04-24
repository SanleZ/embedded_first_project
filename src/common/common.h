#pragma once
#include <stdint.h>

typedef uint16_t gpio_pin_t;

#define BIT_OFFSET(offset) (1U << (offset))
#define N_BIT_OFFSET(offset, n) ((n) << (offset))
#define GPIO_PIN(port_name, pin_num)                                           \
  ((gpio_pin_t)(((port_name) - 'A') << 8) | (pin_num))

static inline uint8_t get_pin_num(gpio_pin_t gpio_pin) {
  return gpio_pin & 0xFF;
}

static inline uint8_t get_port_num(gpio_pin_t gpio_pin) {
  return gpio_pin >> 8;
}

static inline gpio_pin_t get_gpio_pin(char port, uint8_t num) {
  return (((port) - 'A') << 8) | (num);
}
