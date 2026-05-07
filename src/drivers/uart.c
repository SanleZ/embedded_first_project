#include "uart.h"
#include "common/common.h"
#include "drivers/gpio.h"
#include "drivers/rcc.h"
#include "stm32f411xe.h"
#include <stdint.h>

#define SYSTEM_CLOCK_HZ 16000000U

static void uart_configure_gpio(USART_TypeDef *instance, gpio_pin_t gpio_pin_tx,
                                gpio_pin_t gpio_pin_rx) {

  rcc_enable_gpio(gpio_pin_tx);
  rcc_enable_gpio(gpio_pin_rx);

  gpio_set_mode(gpio_pin_tx, GPIO_AF_MODE);
  gpio_set_mode(gpio_pin_rx, GPIO_AF_MODE);

  gpio_set_alternate_function(gpio_pin_tx, GPIO_AF7);
  gpio_set_alternate_function(gpio_pin_rx, GPIO_AF7);
}

static uint32_t rcc_get_apb1_clock(void) { return SYSTEM_CLOCK_HZ; }

static uint32_t rcc_get_apb2_clock(void) { return SYSTEM_CLOCK_HZ; }

static uint32_t uart_get_peripheral_clock(USART_TypeDef *instance) {
  if (instance == USART1) {
    return rcc_get_apb2_clock();
  }
  if (instance == USART2) {
    return rcc_get_apb1_clock();
  }
  return 0;
}

static void uart_set_baudrate(USART_TypeDef *instance, uint32_t periph_clock,
                              uint32_t baudrate) {
  instance->BRR = periph_clock / baudrate;
}

void uart_init(uart_t *uart, USART_TypeDef *instance, uint32_t baudrate) {
  uart->instance = instance;
  if (instance == USART1) {
    rcc_enable_usart1();
    gpio_pin_t tx = get_gpio_pin(GPIO_PORT_A, 9);
    gpio_pin_t rx = get_gpio_pin(GPIO_PORT_A, 10);
    uart_configure_gpio(instance, tx, rx);
  }
  if (instance == USART2) {
    rcc_enable_usart2();
    gpio_pin_t tx = get_gpio_pin(GPIO_PORT_A, 2);
    gpio_pin_t rx = get_gpio_pin(GPIO_PORT_A, 3);
    uart_configure_gpio(instance, tx, rx);
  }
  uart_set_baudrate(instance, uart_get_peripheral_clock(instance), baudrate);

  instance->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
};

void uart_write_byte(uart_t *uart, uint32_t byte) {
  while (!(uart->instance->SR & USART_SR_TXE))
    ;
  uart->instance->DR = byte;
};

void uart_write_string(uart_t *uart, const char *str) {
  while (*str) {
    uart_write_byte(uart, (uint8_t)*str);
    str++;
  }
};
