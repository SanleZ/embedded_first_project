#pragma once
#include "stm32f411xe.h"
#include <stdint.h>

/* Base addresses for NVIC registers */
// Interrupt Set Enable Register
#define NVIC_ISER_BASE 0xE000E100UL
// Interrupt Clear Enable Register
#define NVIC_ICER_BASE 0xE000E180UL

/* NVIC control API */
void nvic_enable_irq(IRQn_Type irq);
void nvic_disable_irq(IRQn_Type irq);
