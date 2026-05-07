#pragma once

#include "stm32f411xe.h"
#include <stdint.h>

typedef struct {
  USART_TypeDef *instance;
} uart_t;

void uart_init(uart_t *uart, USART_TypeDef *instance, uint32_t baudrate);

void uart_write_byte(uart_t *uart, uint32_t byte);

void uart_write_string(uart_t *uart, const char *str);
