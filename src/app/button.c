#include "button.h"

#include "core/event.h"
#include "core/systick.h"
#include "drivers/gpio.h"

void button_init(button_t *btn, gpio_pin_t gpio_pin, uint32_t debounce_ms,
                 button_active_t active_level, uint32_t long_press_ms) {
  btn->pin = gpio_pin;
  btn->debounce_ms = debounce_ms;
  btn->active_level = active_level;
  btn->stable_state = gpio_read(gpio_pin);
  btn->last_change_time = 0;

  btn->press_time = 0;
  btn->long_press_ms = long_press_ms;
  btn->long_press_sent = 0;
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
  if ((ticks - btn->last_change_time) < btn->debounce_ms) {
    return;
  }

  uint8_t current = gpio_read(btn->pin);

  if (btn->stable_state != current) {
    btn->stable_state = current;

    if (button_is_active(btn, current)) {
      btn->press_time = ticks;
      btn->long_press_sent = 0;
      event_push((event_t){.type = EVENT_BUTTON_PRESS, .data = btn->pin.pin});
    } else {
      event_push((event_t){.type = EVENT_BUTTON_RELEASE, .data = btn->pin.pin});
    }
  }

  if (!button_is_active(btn, btn->stable_state)) {
    return;
  }

  // long press
  if (!btn->long_press_sent &&
      (ticks - btn->press_time) >= btn->long_press_ms) {
    btn->long_press_sent = 1;
    event_push(
        (event_t){.type = EVENT_BUTTON_LONG_PRESS, .data = btn->pin.pin});
  }
}
