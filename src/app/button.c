#include "button.h"

#include "core/event.h"
#include "drivers/gpio.h"
#include "drivers/timer.h"
#include <stdint.h>

void button_init(button_t *btn, gpio_pin_t gpio_pin, uint32_t debounce_ms,
                 button_active_t active_level, uint32_t long_press_ms,
                 uint32_t double_click_ms, uint32_t repeat_start_ms,
                 uint32_t repeat_interval_ms, button_id_t id) {

  btn->id = id;
  btn->pin = gpio_pin;
  btn->debounce_ms = debounce_ms;
  btn->active_level = active_level;
  btn->current_state = gpio_read(gpio_pin);
  btn->last_change_time = 0;

  btn->press_time = 0;
  btn->long_press_ms = long_press_ms;
  btn->long_press_event_sent = 0;

  btn->last_release_time = 0;
  btn->double_click_ms = double_click_ms;
  btn->double_click_pending = 0;

  btn->repeat_interval_ms = repeat_interval_ms;
  btn->repeat_start_ms = repeat_start_ms;
  btn->repeat_active = 0;
  btn->repeat_last_time = 0;
}

void button_handle_edge(button_t *btn) {
  btn->last_change_time = timer2_get_ticks();
}

static uint8_t button_is_active(button_t *btn, uint8_t state) {
  if (btn->active_level == BUTTON_ACTIVE_LOW) {
    return state == 0;
  } else {
    return state == 1;
  }
}

void button_update(button_t *btn) {
  uint32_t now = timer2_get_ticks();
  uint8_t current = gpio_read(btn->pin);

  switch (btn->state) {
  case BUTTON_IDLE:
    if (button_is_active(btn, current)) {
      btn->last_change_time = now;
      btn->state = BUTTON_DEBOUNCE;
    }
    break;

  case BUTTON_WAIT_RELEASE:
    if (!button_is_active(btn, current)) {
      btn->state = BUTTON_IDLE;
    }
    break;

  case BUTTON_DEBOUNCE:
    if ((now - btn->last_change_time) >= btn->debounce_ms) {
      if (button_is_active(btn, current)) {
        btn->press_time = now;
        btn->state = BUTTON_PRESSED;
      } else {
        btn->state = BUTTON_IDLE;
      }
    }
    break;
  case BUTTON_PRESSED:
    if (!button_is_active(btn, current)) {
      btn->last_release_time = now;
      btn->state = BUTTON_WAIT_SECOND_CLICK;
    } else if ((now - btn->press_time) >= btn->long_press_ms) {
      event_t e = {.type = EVENT_BUTTON_LONG_PRESS, .data = btn->id};
      event_push(e);
      btn->state = BUTTON_REPEAT;
    }
    break;
  case BUTTON_WAIT_SECOND_CLICK:
    if (button_is_active(btn, current)) {
      btn->last_change_time = now;
      btn->state = BUTTON_DEBOUNCE_SECOND;
    } else if ((now - btn->last_release_time) >= btn->double_click_ms) {
      event_t e = {.type = EVENT_BUTTON_SINGLE_CLICK, .data = btn->id};
      event_push(e);
      btn->state = BUTTON_IDLE;
    }
    break;
  case BUTTON_DEBOUNCE_SECOND:
    if ((now - btn->last_change_time) >= btn->debounce_ms) {

      if (button_is_active(btn, current)) {
        event_t e =
            (event_t){.type = EVENT_BUTTON_DOUBLE_CLICK, .data = btn->id};
        event_push(e);

        btn->state = BUTTON_WAIT_RELEASE;

      } else {
        btn->state = BUTTON_WAIT_SECOND_CLICK;
      }
    }
    break;
  case BUTTON_REPEAT:
    if (!button_is_active(btn, current)) {
      btn->state = BUTTON_IDLE;
    } else if ((now - btn->repeat_last_time) >= btn->repeat_interval_ms) {
      btn->repeat_last_time = now;
      event_t e = {.type = EVENT_BUTTON_REPEAT, .data = btn->id};
      event_push(e);
    }
    break;
  default:
    break;
  }
}
