#include "app/cli.h"
#include "app/led.h"
#include "common/common.h"
#include "common/string_utils.h"
#include "drivers/uart.h"
#include <stdint.h>

static uart_t *cli_uart;
static led_t *cli_led;
static uint8_t cli_index;

static char cli_buffer[CLI_BUFFER_SIZE];

static void cli_cmd_led(cli_args_t *args);

static cli_command_t commands[] = {{"led", cli_cmd_led}};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void cli_cmd_led(cli_args_t *args) {
  if (args->count < 2) {
    uart_write_string(cli_uart, "ERR\r\n");
  }
  char *command = args->tokens[1];
  if (str_compare(command, "on") == 0) {
    led_set_mode(cli_led, LED_MODE_ON);
    uart_write_string(cli_uart, " > LED ON\r\n");
    return;
  } else if (str_compare(command, "off") == 0) {
    led_set_mode(cli_led, LED_MODE_OFF);
    uart_write_string(cli_uart, " > LED OFF\r\n");
    return;
  } else if (str_compare(command, "blink") == 0) {
    uint32_t delay_ms;
    if (str_to_u32(args->tokens[2], &delay_ms)) {
      led_set_mode(cli_led, LED_MODE_BLINK);
      led_set_blink_delay(cli_led, delay_ms);
      char message[64] = " > LED BLINK, DELAY:";
      str_concat(message, args->tokens[2], sizeof(message));
      str_concat(message, "\r\n", sizeof(message));
      uart_write_string(cli_uart, message);
    } else {
      uart_write_string(cli_uart, " > ERR IN BLINK DELAY VALUE\r\n");
    }
  }
}

void cli_init(uart_t *uart, led_t *led) {
  cli_uart = uart;
  cli_led = led;
  cli_index = 0;
}

static void cli_process_command(void) {
  cli_args_t args;
  args.count = 0;
  char *context = COMMON_NULL;
  char *token = str_tokenize(cli_buffer, &context);

  while (token != COMMON_NULL && args.count < MAX_TOKENS) {
    args.tokens[args.count++] = token;
    token = str_tokenize(COMMON_NULL, &context);
  }

  if (args.count == 0) {
    return;
  }

  for (uint32_t i = 0; i < COMMAND_COUNT; i++) {
    if (str_compare(args.tokens[0], commands[i].command) == 0) {
      commands[i].handler(&args);
      return;
    }
  }
  uart_write_string(cli_uart, "UNKNOW COMMAND\r\n");
}

void cli_update(void) {
  uint8_t byte;

  while (uart_read_byte(cli_uart, &byte)) {
    uart_write_byte(cli_uart, byte);
    if (byte == '\r' || byte == '\n') {
      uart_write_string(cli_uart, "\r\n");
      cli_buffer[cli_index] = '\0';
      cli_process_command();
      cli_index = 0;
    } else {
      if (cli_index < CLI_BUFFER_SIZE - 1) {
        cli_buffer[cli_index++] = (char)byte;
      }
    }
  }
}
