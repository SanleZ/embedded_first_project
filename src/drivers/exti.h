#pragma once
#include "common/common.h"
#include <stdint.h>

#define EXTI_BASE 0x40013C00UL

typedef enum {
  EXTI_TRIGGER_RISING,
  EXTI_TRIGGER_FALLING,
  EXTI_TRIGGER_BOTH
} exti_trigger_t;

typedef struct {
  volatile uint32_t IMR, EMR, RTSR, FTSR, SWIER, PR;
} exti_t;

#define EXTI ((exti_t *)(uintptr_t)EXTI_BASE)

void exti_configure_line(gpio_pin_t pin, exti_trigger_t trigger);
void exti_enable_line(gpio_pin_t pin);
void exti_clear_pending(gpio_pin_t pin);
