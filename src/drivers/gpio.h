#pragma once
#include "common/common.h"
#include <stdint.h>

#define GPIO_BASE 0x40020000

typedef struct {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL,
      AFRH;
} gpio_t;

#define GPIO(port_num) ((gpio_t *)((uintptr_t)GPIO_BASE + 0x400 * (port_num)))

typedef enum {
  GPIO_INPUT_MODE,  // 00
  GPIO_OUTPUT_MODE, // 01
  GPIO_AF_MODE,     // 02
  GPIO_ANALOG_MODE  // 03
} gpio_mode_t;

typedef enum { GPIO_NO_PULL = 0, GPIO_PULLUP, GPIO_PULLDOWN } gpio_pull_t;

void gpio_set_mode(gpio_pin_t gpio_pin, gpio_mode_t mode);
void gpio_set_pull(gpio_pin_t gpio_pin, gpio_pull_t pull_type);
void gpio_write(gpio_pin_t gpio_pin, uint8_t value);
void gpio_reset(gpio_pin_t gpio_pin);
void gpio_set(gpio_pin_t gpio_pin);
void gpio_toggle(gpio_pin_t gpio_pin);
