#pragma once

#include "app/led.h"
#include "drivers/uart.h"
#include <stdint.h>

#define CLI_BUFFER_SIZE 64
#define MAX_TOKENS 4

typedef struct {
  char *tokens[MAX_TOKENS];
  uint8_t count;
} cli_args_t;

typedef void (*cli_command_handler_t)(cli_args_t *args);

typedef struct {
  const char *command;
  cli_command_handler_t handler;
} cli_command_t;

void cli_init(uart_t *uart, led_t *led);
void cli_update(void);
