#pragma once
#include <stdint.h>

extern volatile uint32_t ticks;
/* SysTick Regs */

typedef struct {
  volatile uint32_t CTRL;  // 0x00
  volatile uint32_t LOAD;  // 0x04
  volatile uint32_t VAL;   // 0x08
  volatile uint32_t CALIB; // 0x0C
} systick_t;

#define SYSTICK ((systick_t *)0xE000E010UL)

/* CTRL register bits */
#define SYSTICK_CTRL_ENABLE (1U << 0) // Enables the SysTick counter
#define SYSTICK_CTRL_TICKINT                                                   \
  (1U << 1) // Enables SysTick interrupt on count to zero
#define SYSTICK_CTRL_CLKSOURCE_AHB (1U << 2) // Clock source-CPU clock
#define SYSTICK_CTRL_COUNTFLAG                                                 \
  (1U << 16) // Set to 1 when the counter reaches zero

typedef enum {
  SYSTICK_SRC_AHB_DIV8 = 0U,                   // clock source - external clock
  SYSTICK_SRC_AHB = SYSTICK_CTRL_CLKSOURCE_AHB // CPU clock
} systick_clksource_t;

/* High-level API */

// initialization
void systick_init(uint32_t ticks, systick_clksource_t src);

// delay in ms
void systick_delay_ms(uint32_t ms);

uint32_t systick_get_ticks();

// interrupt handler
void SysTick_Handler(void);
