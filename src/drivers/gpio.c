#include "gpio.h"
#include "common/common.h"
#include <stdint.h>

// setting the GPIO mode
void gpio_set_mode(gpio_pin_t gpio_pin, gpio_mode_t mode) {
  uint8_t port_num = get_port_num(gpio_pin);
  uint8_t shift = get_pin_num(gpio_pin) * 2;
  gpio_t *gpio = GPIO(port_num);
  gpio->MODER &= ~(3U << (shift));         // clear 2 bits
  gpio->MODER |= ((mode & 3U) << (shift)); // set pin-each pin takes up two
}

void gpio_set_pull(gpio_pin_t gpio_pin, gpio_pull_t pull) {
  gpio_t *gpio = GPIO(get_port_num(gpio_pin));
  uint8_t shift = get_pin_num(gpio_pin) * 2;
  gpio->PUPDR &= ~(3U << shift);
  gpio->PUPDR |= ((pull & 3U) << shift);
}

void gpio_set(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(get_port_num(gpio_pin));
  gpio->BSRR = BIT_OFFSET(get_pin_num(gpio_pin));
}

void gpio_reset(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(get_port_num(gpio_pin));
  gpio->BSRR = BIT_OFFSET(get_pin_num(gpio_pin) + 16);
}

void gpio_write(gpio_pin_t gpio_pin, uint8_t value) {
  if (value) {
    gpio_reset(gpio_pin);
  } else {
    gpio_set(gpio_pin);
  }
}

void gpio_toggle(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(get_port_num(gpio_pin));
  gpio->ODR ^= BIT_OFFSET(get_pin_num(gpio_pin));
}
