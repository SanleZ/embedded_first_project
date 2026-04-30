#pragma once
#include <stdint.h>

// #define EXTI_BASE 0x40013C00UL

typedef enum {
  EXTI_TRIGGER_RISING,
  EXTI_TRIGGER_FALLING,
  EXTI_TRIGGER_BOTH
} exti_trigger_t;

/* Type for user-defined interrupt callbacks */
typedef void (*exti_callback_t)(void);

typedef struct {
  volatile uint32_t IMR, // Interrupt Mask Register
      EMR, RTSR, FTSR, SWIER, PR;
} exti_t;

// #define EXTI ((exti_t *)(uintptr_t)EXTI_BASE)

void exti_configure_line(uint8_t line, exti_trigger_t trigger);
void exti_enable_line(uint8_t line);
void exti_disable_line(uint8_t line);
void exti_clear_pending(uint8_t line);
uint8_t exti_is_pending(uint8_t line);

void exti_register_callback(uint8_t line, exti_callback_t cb);
void exti_handle_irq(uint8_t);
