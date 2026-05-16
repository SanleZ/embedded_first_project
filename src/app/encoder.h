#pragma once
#include "common/common.h"
#include <stdint.h>
typedef enum {
  ENCODER_EVENT_NONE = 0,
  ENCODER_EVENT_CW,
  ENCODER_EVENT_CCW,
  ENCODER_EVENT_BUTTON_CLICK
} encoder_event_t;

typedef struct {
  gpio_pin_t pin_a;
  gpio_pin_t pin_b;

  uint32_t last_state;
  int32_t position;
  int8_t delta;
} encoder_t;

void encoder_init(encoder_t *enc, gpio_pin_t pin_a, gpio_pin_t pin_b);

encoder_event_t encoder_update(encoder_t *enc);

void encoder_hw_init(void);

void task_encoder(void);
