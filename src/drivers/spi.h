#pragma once
#include "stm32f411xe.h"
#include <stdint.h>
void spi1_init(void);

static inline void spi1_write_byte(uint8_t byte) {

  while (!(SPI1->SR & SPI_SR_TXE)) {
  }

  *((volatile uint8_t *)&SPI1->DR) = byte;
}

static inline void spi1_write_word(uint16_t word) {

  while (!(SPI1->SR & SPI_SR_TXE)) {
  }

  *((volatile uint16_t *)&SPI1->DR) = word;
}

static inline void spi1_wait(void) {

  while (SPI1->SR & SPI_SR_BSY) {
  }
}

static inline void spi1_set_8bit(void) {
  spi1_wait();
  SPI1->CR1 &= ~SPI_CR1_SPE;
  SPI1->CR1 &= ~SPI_CR1_DFF;
  SPI1->CR1 |= SPI_CR1_SPE;
}

static inline void spi1_set_16bit(void) {
  spi1_wait();
  SPI1->CR1 &= ~SPI_CR1_SPE;
  SPI1->CR1 |= SPI_CR1_DFF;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_write_dma(const uint16_t *data, uint32_t count);
