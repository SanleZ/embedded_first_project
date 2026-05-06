#include "systick.h"
#include <stdint.h>

volatile uint32_t ticks = 0;

void systick_init(uint32_t ticks, systick_clksource_t src) {
  SYSTICK->LOAD = ticks - 1;
  SYSTICK->VAL = 0;
  SYSTICK->CTRL = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT | src;
}

uint32_t systick_get_ticks() { return ticks; }

void SysTick_Handler(void) { ticks++; }
