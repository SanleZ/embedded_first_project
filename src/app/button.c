#include "button.h"

#include "core/event.h"
#include "core/systick.h"
#include "drivers/gpio.h"
#include <stdint.h>

void button_init(button_t *btn, gpio_pin_t gpio_pin, uint32_t debounce_ms,
                 button_active_t active_level, uint32_t long_press_ms,
                 uint32_t double_click_ms, uint32_t repeat_start_ms,
                 uint32_t repeat_interval_ms) {
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

void button_handle_edge(button_t *btn) { btn->last_change_time = ticks; }

static uint8_t button_is_active(button_t *btn, uint8_t state) {
  if (btn->active_level == BUTTON_ACTIVE_LOW) {
    return state == 0;
  } else {
    return state == 1;
  }
}

void button_update(button_t *btn) {
  // debounce
  if ((ticks - btn->last_change_time) < btn->debounce_ms) {
    return;
  }

  uint8_t current_button_gpio_state = gpio_read(btn->pin);

  // state has been changed
  if (btn->current_state != current_button_gpio_state) {

    btn->current_state = current_button_gpio_state;

    if (button_is_active(btn, current_button_gpio_state)) {

      // pressed
      btn->press_time = ticks;
      btn->long_press_event_sent = 0;
      btn->repeat_active = 0;

    } else {

      // released
      if (btn->long_press_event_sent) {
        // ther was a long press - ignore clicks
        btn->double_click_pending = 0;
      } else {

        if (btn->double_click_pending &&
            (ticks - btn->last_release_time <= btn->double_click_ms)) {

          // DOUBLE CLICK
          btn->double_click_pending = 0;

          event_push((event_t){.type = EVENT_BUTTON_DOUBLE_CLICK,
                               .data = btn->pin.pin});

        } else {
          // wait for the second click
          btn->double_click_pending = 1;
          btn->last_release_time = ticks;
        }
      }
    }
  }

  // --- long press ---
  if (button_is_active(btn, btn->current_state)) {

    uint32_t held_time = ticks - btn->press_time;

    if (!btn->long_press_event_sent && (held_time >= btn->long_press_ms)) {
      btn->long_press_event_sent = 1;
      btn->double_click_pending = 0;
      event_push(
          (event_t){.type = EVENT_BUTTON_LONG_PRESS, .data = btn->pin.pin});
    }

    if (!btn->repeat_active && held_time >= btn->repeat_start_ms) {
      btn->repeat_active = 1;
      btn->repeat_last_time = ticks;
    }

    if (btn->repeat_active &&
        (ticks - btn->repeat_last_time >= btn->repeat_interval_ms)) {
      btn->repeat_last_time = ticks;
      event_push((event_t){.type = EVENT_BUTTON_REPEAT, .data = btn->pin.pin});
    }
  } else {
    btn->repeat_active = 0;
  }

  // --- fisnish single click ---
  if (btn->double_click_pending &&
      (ticks - btn->last_release_time > btn->double_click_ms)) {

    btn->double_click_pending = 0;

    event_push(
        (event_t){.type = EVENT_BUTTON_SINGLE_CLICK, .data = btn->pin.pin});
  }
}
