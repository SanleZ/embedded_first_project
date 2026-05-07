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

typedef enum {
  GPIO_AF0 = 0,
  GPIO_AF1 = 1,
  GPIO_AF2 = 2,
  GPIO_AF3 = 3,
  GPIO_AF4 = 4,
  GPIO_AF5 = 5,
  GPIO_AF6 = 6,
  GPIO_AF7 = 7,
  GPIO_AF8 = 8,
  GPIO_AF9 = 9,
  GPIO_AF10 = 10,
  GPIO_AF11 = 11,
  GPIO_AF12 = 12,
  GPIO_AF13 = 13,
  GPIO_AF14 = 14,
  GPIO_AF15 = 15
} gpio_alternate_function_t;

typedef enum { GPIO_NO_PULL = 0, GPIO_PULLUP, GPIO_PULLDOWN } gpio_pull_t;

void gpio_set_mode(gpio_pin_t gpio_pin, gpio_mode_t mode);
void gpio_set_pull(gpio_pin_t gpio_pin, gpio_pull_t pull_type);
uint8_t gpio_read(gpio_pin_t gpio_pin);
void gpio_write(gpio_pin_t gpio_pin, uint8_t value);
void gpio_reset(gpio_pin_t gpio_pin);
void gpio_set(gpio_pin_t gpio_pin);
void gpio_toggle(gpio_pin_t gpio_pin);
void gpio_set_alternate_function(gpio_pin_t gpio_pin,
                                 gpio_alternate_function_t af);
