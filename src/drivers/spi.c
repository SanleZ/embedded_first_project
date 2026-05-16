#include "drivers/spi.h"
#include "stm32f411xe.h"

void spi1_init(void) {
  // 1. Тактирование
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

  /*
   * PB3 -> SPI1_SCK
   * PB5 -> SPI1_MOSI
   * AF5
   */

  GPIOB->MODER &= ~((3U << (3 * 2)) | (3U << (5 * 2)) | (3U << (7 * 2)) |
                    (3U << (8 * 2))); // Сброс
  GPIOB->MODER |= (2U << (3 * 2)) | (2U << (5 * 2)) | (1U << (7 * 2)) |
                  (1U << (8 * 2)); // Режимы

  GPIOB->OSPEEDR |= (3U << (3 * 2)) | (3U << (5 * 2));
  GPIOB->AFR[0] |= (5U << (3 * 4));
  GPIOB->AFR[0] |= (5U << (5 * 4));

  // 3. Настройка SPI1 (Сброс и чистая установка)
  SPI1->CR1 = 0;
  // Master, BR /64 (безопасно), Software CS, Mode 3 (CPOL=1, CPHA=1)
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPOL |
               SPI_CR1_CPHA | SPI_CR1_DFF;
  SPI1->CR2 = 0;
  SPI1->CR1 |= SPI_CR1_SPE;

  SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

// void spi1_write_byte(uint8_t byte) {
//
//   *((volatile uint8_t *)&SPI1->DR) = byte;
//   while (!(SPI1->SR & SPI_SR_TXE)) {
//   }
//
//   while ((SPI1->SR & SPI_SR_BSY)) {
//   }
// }

void spi1_write_dma(const uint16_t *data, uint32_t count) {

  DMA2_Stream3->CR &= ~DMA_SxCR_EN;

  while (DMA2_Stream3->CR & DMA_SxCR_EN) {
  }

  DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 |
                DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;

  DMA2_Stream3->PAR = (uint32_t)&SPI1->DR;

  DMA2_Stream3->M0AR = (uint32_t)data;

  DMA2_Stream3->NDTR = count;

  DMA2_Stream3->CR = (3U << DMA_SxCR_CHSEL_Pos) | // Channel 3
                     DMA_SxCR_DIR_0 |             // Memory -> Peripheral
                     DMA_SxCR_MINC |              // Increment memory
                     DMA_SxCR_MSIZE_0 |           // 16-bit memory
                     DMA_SxCR_PSIZE_0;            // 16-bit peripheral

  DMA2_Stream3->CR |= DMA_SxCR_EN;

  while (!(DMA2->LISR & DMA_LISR_TCIF3)) {
  }

  while (SPI1->SR & SPI_SR_BSY) {
  }
}
