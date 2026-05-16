#include "rcc.h"
#include "common/common.h"
#include "stm32f411xe.h"

void rcc_enable_gpio(gpio_port_t gpio_port) {
  uint8_t port_num = gpio_port;
  RCC->AHB1ENR |= BIT_OFFSET(port_num);
}

void rcc_enable_syscfg(void) { RCC->APB2ENR |= BIT_OFFSET(14); }

void rcc_enable_usart1(void) { RCC->APB2ENR |= RCC_APB2ENR_USART1EN; }
void rcc_enable_usart2(void) { RCC->APB1ENR |= RCC_APB1ENR_USART2EN; }

void rcc_enable_timer3(void) { RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; }

void rcc_enable_spi1(void) { RCC->AHB1ENR |= RCC_APB1ENR_SPI2EN; }
