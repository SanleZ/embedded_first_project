#pragma once
#include "common/common.h"
#include <stdint.h>

typedef enum {
  LED_MODE_ON,
  LED_MODE_OFF,
  LED_MODE_BLINK,
} led_mode_t;

typedef enum { LED_STATE_OFF = 0, LED_STATE_ON } led_state_t;

typedef struct {
  led_state_t state;
  uint32_t last_toggle;
  led_mode_t mode;
  gpio_pin_t gpio_pin;
  uint32_t blink_period_ms;
} led_t;

void led_init(led_t *led, gpio_pin_t gpio_pin);

void led_set_mode(led_t *led, led_mode_t mode);

void led_update(led_t *led);

void led_on(led_t *led);

void led_off(led_t *led);

void led_toggle(led_t *led);

void led_set_blink_delay(led_t *led, uint32_t delay);
