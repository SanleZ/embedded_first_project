#pragma once
#include <stdint.h>
extern volatile uint32_t timer_ticks;
void timer2_init(void);
uint32_t timer2_get_ticks(void);

void timer3_pwm_init(void);
