#pragma once

#include <stdint.h>

#include "common/common.h"

typedef enum { BUTTON_ACTIVE_LOW = 0, BUTTON_ACTIVE_HIGH } button_active_t;

typedef struct {
  gpio_pin_t pin;
  uint8_t stable_state;
  uint8_t last_raw_state;

  uint32_t last_change_time;
  uint32_t debounce_ms;
  button_active_t active_level;
} button_t;

void button_init(button_t *btn, gpio_pin_t gpio_pin, uint32_t debounce_ms,
                 button_active_t active_level);

void button_handle_edge(button_t *btn);

void button_update(button_t *btn);
