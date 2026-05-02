#pragma once

#include <stdint.h>

#include "common/common.h"

typedef enum {
  BUTTON_ID_SWITCH,
  BUTTON_ID_OK,
  BUTTTON_ID_RESET,
  BUTTON_ID_CANCEL,
  BUTTON_ID_UP,
  BUTTON_ID_DOWN,
  BUTTON_ID_LEFT,
  BUTTON_ID_RIGHT
} button_id_t;

typedef enum {
  BUTTON_IDLE,
  BUTTON_DEBOUNCE,
  BUTTON_PRESSED,
  BUTTON_WAIT_SECOND_CLICK,
  BUTTON_WAIT_RELEASE,
  BUTTON_REPEAT,
  BUTTON_DEBOUNCE_SECOND
} button_state_t;

typedef enum { BUTTON_ACTIVE_LOW = 0, BUTTON_ACTIVE_HIGH } button_active_t;

typedef struct {
  button_id_t id;
  gpio_pin_t pin;
  uint8_t current_state;
  uint8_t last_raw_state;

  uint32_t last_change_time;
  uint32_t debounce_ms;
  button_active_t active_level;

  // long press
  uint32_t press_time;
  uint32_t long_press_ms;
  uint8_t long_press_event_sent;

  // double click
  uint32_t last_release_time;
  uint32_t double_click_ms;
  uint32_t double_click_pending;

  // auto-repeat
  uint32_t repeat_start_ms;
  uint32_t repeat_interval_ms;

  uint32_t repeat_last_time;
  uint8_t repeat_active;
  button_state_t state;

} button_t;

void button_init(button_t *btn, gpio_pin_t gpio_pin, uint32_t debounce_ms,
                 button_active_t active_level, uint32_t long_press_ms,
                 uint32_t double_click_ms, uint32_t repeat_start_ms,
                 uint32_t repeat_interval_ms, button_id_t id);

void button_handle_edge(button_t *btn);

void button_update(button_t *btn);
