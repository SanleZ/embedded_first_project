#include "exti.h"
#include "common/common.h"
#include "stm32f411xe.h"
#include <stdint.h>

/*
 * Static array holding callbacks for each EXTI line (0–15).
 * Index directly corresponds to EXTI line number.
 */
static exti_callback_t callbacks[16] = {0};

void exti_configure_line(uint8_t line, exti_trigger_t trigger) {
  if (line > 15) {
    return;
  }
  EXTI->RTSR &= ~BIT_OFFSET(line);
  EXTI->FTSR &= ~BIT_OFFSET(line);

  switch (trigger) {
  case EXTI_TRIGGER_RISING:
    EXTI->RTSR |= BIT_OFFSET(line);
    break;
  case EXTI_TRIGGER_FALLING:
    EXTI->FTSR |= BIT_OFFSET(line);
    break;
  case EXTI_TRIGGER_BOTH:
    EXTI->RTSR |= BIT_OFFSET(line);
    EXTI->FTSR |= BIT_OFFSET(line);
    break;
  }
}

void exti_enable_line(uint8_t line) {
  if (line > 15)
    return;
  EXTI->IMR |= BIT_OFFSET(line);
}

void exti_disable_line(uint8_t line) {
  if (line > 15)
    return;
  EXTI->IMR &= ~BIT_OFFSET(line);
}

/*
 * Checks whether an interrupt is pending for a given EXTI line.
 *
 * EXTI->PR (Pending Register):
 * - Bit is set by hardware when interrupt condition occurs
 * - Must be cleared manually by software
 */
uint8_t exti_is_pending(uint8_t line) {
  if (line > 15)
    return 0;
  return (EXTI->PR & BIT_OFFSET(line)) != 0;
}

/*
 * Clears the pending interrupt flag for a given EXTI line.
 *
 * IMPORTANT:
 * - PR is a "write 1 to clear" register
 * - Writing 0 has no effect
 * - Writing 1 clears the corresponding pending bit
 */
void exti_clear_pending(uint8_t line) {
  if (line > 15)
    return;
  /* Writing '1' to the bit clears it (does NOT set it) */
  EXTI->PR = BIT_OFFSET(line);
}

/* --- Callback registration --- */

/*
 * Registers a user callback for a specific EXTI line.
 *
 * This callback will be executed when the corresponding interrupt fires.
 */
void exti_register_callback(uint8_t line, exti_callback_t cb) {
  if (line > 15)
    return;
  callbacks[line] = cb;
}

/* --- IRQ dispatcher --- */

/*
 * Handles interrupt for a specific EXTI line.
 *
 * This function is intended to be called from the actual ISR
 * (e.g., EXTI0_IRQHandler, EXTI9_5_IRQHandler, etc.).
 *
 * Responsibilities:
 * 1. Check if interrupt is actually pending
 * 2. Clear pending flag
 * 3. Call user-registered callback (if any)
 */
void exti_handle_irq(uint8_t line) {
  if (line > 15)
    return;
  if (exti_is_pending(line)) {
    exti_clear_pending(line);

    if (callbacks[line]) {
      callbacks[line]();
    }
  }
}
