#include "app/encoder.h"

#include "drivers/gpio.h"
#include "stm32f411xe.h"
#include <stdint.h>

static uint16_t brightness = 50;
static const uint16_t MIN_BRIGHTNESS = 0;
static const uint16_t MAX_BRIGHTNESS = 1000;
static const uint16_t BRIGHTNESS_STEP = 50;

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

static int16_t last_encoder_pos = 0;

void encoder_hw_init(void) {

  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

  // PB6 / PB7 -> AF2
  GPIOB->MODER &= ~(GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);

  GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

  GPIOB->AFR[0] &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));

  GPIOB->AFR[0] |= ((2 << (6 * 4)) | (2 << (7 * 4)));

  // pullup
  GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6_Msk | GPIO_PUPDR_PUPD7_Msk);

  GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);

  // CH1 input
  TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;

  // CH2 input
  TIM4->CCMR1 |= TIM_CCMR1_CC2S_0;

  // filter
  TIM4->CCMR1 |=
      ((0b1111 << TIM_CCMR1_IC1F_Pos) | (0b1111 << TIM_CCMR1_IC2F_Pos));

  // encoder mode 3
  TIM4->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

  TIM4->ARR = 0xFFFF;
  TIM4->CNT = 0;

  TIM4->CR1 |= TIM_CR1_CEN;
}

void task_encoder(void) {

  int16_t pos = TIM4->CNT;

  int16_t delta = pos - last_encoder_pos;

  if (delta == 0) {
    return;
  }

  last_encoder_pos = pos;

  brightness += delta * BRIGHTNESS_STEP;

  if (brightness > MAX_BRIGHTNESS) {
    brightness = MAX_BRIGHTNESS;
  }

  if (brightness < MIN_BRIGHTNESS) {
    brightness = MIN_BRIGHTNESS;
  }

  TIM3->CCR3 = brightness;
}
