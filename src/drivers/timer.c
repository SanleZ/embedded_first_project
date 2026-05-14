#include "drivers/timer.h"
#include "drivers/rcc.h"
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

uint32_t timer2_get_ticks() { return timer_ticks; }

void timer3_pwm_init(void) {
  rcc_enable_timer3();
  TIM3->PSC = 15;
  TIM3->ARR = 1000;
  TIM3->CCR3 = 500;
  TIM3->CCMR2 |= (6 << 4);
  TIM3->CCMR2 |= TIM_CCMR2_OC3PE;
  TIM3->CCER |= TIM_CCER_CC3E;
  TIM3->CR1 |= TIM_CR1_ARPE;
  TIM3->EGR |= TIM_EGR_UG;
  TIM3->CR1 |= TIM_CR1_CEN;
}
