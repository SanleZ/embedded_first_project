#pragma once
#include "common/common.h"
#include <stdint.h>

// #define SYSCFG_BASE 0x40013800UL

typedef struct {
  volatile uint32_t MEMRMP;
  volatile uint32_t PMC;
  volatile uint32_t EXTICR[4];
  volatile uint32_t CMPCR;
} syscfg_t;

// #define SYSCFG ((syscfg_t *)(uintptr_t)SYSCFG_BASE)

void syscfg_map_exti_line(gpio_pin_t pin);
