#pragma once
#include <stdint.h>

typedef enum { GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C } gpio_port_t;

#define COMMON_NULL ((void *)0)

typedef struct {
  gpio_port_t port;
  uint16_t pin;
} gpio_pin_t;

#define BIT_OFFSET(offset) (1U << ((offset) & 31))
#define N_BIT_OFFSET(offset, n) ((uint32_t)(n) << (offset))

static inline gpio_pin_t get_gpio_pin(gpio_port_t port, uint8_t num) {
  return (gpio_pin_t){.port = port, .pin = (num)};
}
