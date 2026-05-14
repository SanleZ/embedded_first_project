#include "app/encoder.h"

#include "drivers/gpio.h"
#include <stdint.h>

void encoder_init(encoder_t *enc, gpio_pin_t pin_a, gpio_pin_t pin_b) {
  enc->pin_a = pin_a;
  enc->pin_b = pin_b;

  uint8_t a = gpio_read(pin_a);
  uint8_t b = gpio_read(pin_b);
  enc->last_state = (a << 1) | b;
  enc->position = 0;
}

encoder_event_t encoder_update(encoder_t *enc) {
  uint8_t a = gpio_read(enc->pin_a);
  uint8_t b = gpio_read(enc->pin_b);

  uint8_t current_state = (a << 1) | b;
  if (current_state == enc->last_state) {
    return ENCODER_EVENT_NONE;
  }
  encoder_event_t event = ENCODER_EVENT_NONE;

  switch (enc->last_state) {

  case 0b00:
    if (current_state == 0b01) {
      event = ENCODER_EVENT_CW;
    } else if (current_state == 0b10) {
      event = ENCODER_EVENT_CCW;
    }
    break;

  case 0b01:
    if (current_state == 0b11) {
      event = ENCODER_EVENT_CW;
    } else if (current_state == 0b00) {
      event = ENCODER_EVENT_CCW;
    }
    break;

  case 0b11:
    if (current_state == 0b10) {
      event = ENCODER_EVENT_CW;
    } else if (current_state == 0b01) {
      event = ENCODER_EVENT_CCW;
    }
    break;

  case 0b10:
    if (current_state == 0b00) {
      event = ENCODER_EVENT_CW;
    } else if (current_state == 0b11) {
      event = ENCODER_EVENT_CCW;
    }
    break;
  }

  enc->last_state = current_state;

  if (event == ENCODER_EVENT_CW) {
    enc->delta++;
  }

  if (event == ENCODER_EVENT_CCW) {
    enc->delta--;
  }

  if (current_state == 0b00) {
    if (enc->delta >= 2) {
      enc->delta = 0;
      enc->position++;
      return ENCODER_EVENT_CW;
    }
    if (enc->delta <= -2) {
      enc->delta = 0;
      enc->position--;
      return ENCODER_EVENT_CCW;
    }
    enc->delta = 0;
  }

  return ENCODER_EVENT_NONE;
}
