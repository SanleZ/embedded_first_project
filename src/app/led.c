#include "led.h"
#include "core/systick.h"
#include "drivers/gpio.h"
#include <stdint.h>
#include <sys/types.h>

static uint32_t led_get_period(led_mode_t mode) {
  switch (mode) {
  case LED_MODE_ALWAYS_ON:
    return 0;
    break;
  case LED_MODE_ALWAYS_OFF:
    return -1;
    break;
  case LED_MODE_BLINK_FAST:
    return 100;
    break;
  case LED_MODE_BLINK_SLOW:
    return 300;
    break;
  case LED_MODE_BLINK_SUPER_FAST:
    return 50;
    break;
  case LED_MODE_BLINK_SUPER_SLOW:
    return 1000;
    break;
  default:
    return 0;
  }
}

void led_init(led_t *led, gpio_pin_t gpio_pin) {
  led->gpio_pin = gpio_pin;
  led->mode = LED_MODE_ALWAYS_ON;
  led->state = LED_STATE_ON;
  led->last_toggle = 0;
}

void led_off(led_t *led) { led->state = LED_STATE_OFF; }

void led_on(led_t *led) { led->state = LED_STATE_ON; }

void led_set_mode(led_t *led, led_mode_t mode) { led->mode = mode; }

void led_update(led_t *led) {
  switch (led->mode) {
  case LED_MODE_ALWAYS_ON:
    gpio_reset(led->gpio_pin);
    break;
  case LED_MODE_ALWAYS_OFF:
    gpio_set(led->gpio_pin);
    break;
  default: {
    uint32_t period = led_get_period(led->mode);
    uint32_t now = systick_get_ticks();

    if (period < 0) {
      gpio_write(led->gpio_pin, 0);
    } else if ((now - led->last_toggle) >= period) {
      led->last_toggle = now;
      led->state = !led->state;
      gpio_write(led->gpio_pin, led->state);
    }
    break;
  }
  }
}
