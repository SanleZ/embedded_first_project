#include "drivers/timer.h"
#include "stm32f411xe.h"
#include <stdint.h>

volatile uint32_t timer_ticks = 0;

void timer2_init(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  TIM2->PSC = 15999;
  TIM2->ARR = 1;

  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) {
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~TIM_SR_UIF;
    timer_ticks++;
  }
}

uint32_t timer_get_ticks() { return timer_ticks; }
