#include "led.h"
#include "core/systick.h"
#include "drivers/gpio.h"
#include <stdint.h>

void led_init(led_t *led, gpio_pin_t gpio_pin) {
  led->gpio_pin = gpio_pin;
  led->mode = LED_MODE_ON;
  led->state = LED_STATE_ON;
  led->last_toggle = 0;
}

void led_off(led_t *led) { led->state = LED_STATE_OFF; }

void led_on(led_t *led) { led->state = LED_STATE_ON; }

void led_set_mode(led_t *led, led_mode_t mode) { led->mode = mode; }

void led_update(led_t *led) {
  switch (led->mode) {
  case LED_MODE_ON:
    gpio_reset(led->gpio_pin);
    break;
  case LED_MODE_OFF:
    gpio_set(led->gpio_pin);
    break;
  default: {
    uint32_t period = led->blink_period_ms;
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

void led_set_blink_delay(led_t *led, uint32_t delay_ms) {
  led->blink_period_ms = delay_ms < 10000 ? delay_ms : 10000;
}
