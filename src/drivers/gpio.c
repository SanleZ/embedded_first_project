#include "gpio.h"
#include "common/common.h"
#include <stdint.h>

// setting the GPIO mode
void gpio_set_mode(gpio_pin_t gpio_pin, gpio_mode_t mode) {
  gpio_port_t port_num = gpio_pin.port;
  uint8_t shift = gpio_pin.pin * 2;
  gpio_t *gpio = GPIO(port_num);
  gpio->MODER &= ~(3U << (shift));         // clear 2 bits
  gpio->MODER |= ((mode & 3U) << (shift)); // set pin-each pin takes up two
}

void gpio_set_pull(gpio_pin_t gpio_pin, gpio_pull_t pull) {
  gpio_t *gpio = GPIO(gpio_pin.port);
  uint8_t shift = gpio_pin.pin * 2;
  gpio->PUPDR &= ~(3U << shift);
  gpio->PUPDR |= ((pull & 3U) << shift);
}

void gpio_set(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(gpio_pin.port);
  gpio->BSRR = BIT_OFFSET(gpio_pin.pin);
}

void gpio_reset(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(gpio_pin.port);
  gpio->BSRR = BIT_OFFSET(gpio_pin.pin + 16);
}

uint8_t gpio_read(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(gpio_pin.port);
  return (gpio->IDR & (BIT_OFFSET(gpio_pin.pin))) ? 1 : 0;
}

void gpio_write(gpio_pin_t gpio_pin, uint8_t value) {
  if (value) {
    gpio_reset(gpio_pin);
  } else {
    gpio_set(gpio_pin);
  }
}

void gpio_toggle(gpio_pin_t gpio_pin) {
  gpio_t *gpio = GPIO(gpio_pin.port);
  gpio->ODR ^= BIT_OFFSET(gpio_pin.pin);
}
