#pragma once
#include "common/common.h"
#include <stdint.h>

// #define RCC_BASE 0x40023800UL

typedef struct {
  volatile uint32_t CR, PLLCCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, RESERVED0[2],
      APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, RESERVED2[2], APB1ENR,
      APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR, RESERVED4[2], APB1LPENR,
      APB2LPENR, RESERVED5[2], BDCR, CSR, RESERVED6[2], SSCGR, RESERVED7[2],
      PLLI2SCFGR;
} rcc_t;

// #define RCC ((rcc_t *)((uintptr_t)RCC_BASE))

void rcc_enable_gpio(gpio_port_t gpio_port); // Enable clocks for GPIO port
void rcc_enable_syscfg(void);                // Enable SYSCFG clock for EXTI
void rcc_enable_usart1(void);
void rcc_enable_usart2(void);
void rcc_enable_timer3(void);
void rcc_enable_spi1(void);
