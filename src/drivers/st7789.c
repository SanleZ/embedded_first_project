#include "drivers/st7789.h"

#include "drivers/gpio.h"
#include "drivers/rcc.h"
#include "drivers/spi.h"

#include "stm32f411xe.h"

static uint16_t framebuffer[240 * 40];
static uint16_t rect_buffer[24 * 20];
static gpio_pin_t cs_pin;
static gpio_pin_t dc_pin;
static gpio_pin_t rst_pin;

static void cs_low(void) { gpio_reset(cs_pin); }

static void cs_high(void) { gpio_set(cs_pin); }

static void dc_command(void) { gpio_reset(dc_pin); }

static void dc_data(void) { gpio_set(dc_pin); }

static void rst_low(void) { gpio_reset(rst_pin); }

static void rst_high(void) { gpio_set(rst_pin); }

void fb_clear(uint16_t color) {
  for (uint32_t i = 0; i < 240 * 40; i++) {
    framebuffer[i] = color;
  }
}

void fb_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {

  for (uint16_t yy = 0; yy < h; yy++) {
    for (uint16_t xx = 0; xx < w; xx++) {
      uint16_t px = x + xx;
      uint16_t py = y + yy;

      if (px >= 240 || py >= 40) {
        continue;
      }
      framebuffer[py * 240 + px] = color;
    }
  }
}

static void st7789_write_command(uint8_t cmd) {
  spi1_set_8bit();
  dc_command();
  spi1_write_byte(cmd);
}

static void st7789_write_data(uint8_t data) {
  dc_data();
  spi1_write_byte(data);
}
static inline void st7789_write_data_fast(uint8_t data) {
  spi1_write_byte(data);
}

static void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1,
                              uint16_t y1) {

  st7789_write_command(0x2A);

  st7789_write_data(x0 >> 8);
  st7789_write_data(x0 & 0xFF);
  st7789_write_data(x1 >> 8);
  st7789_write_data(x1 & 0xFF);

  st7789_write_command(0x2B);

  st7789_write_data(y0 >> 8);
  st7789_write_data(y0 & 0xFF);
  st7789_write_data(y1 >> 8);
  st7789_write_data(y1 & 0xFF);

  st7789_write_command(0x2C);
}

static uint16_t line_buffer[240];

void st7789_fill_color(uint16_t color) {
  st7789_set_window(0, 0, 239, 239);
  spi1_wait();
  spi1_set_16bit();

  dc_data();

  for (uint32_t i = 0; i < 240 * 240; i++) {

    spi1_write_word(color);
  }

  spi1_wait();
  spi1_set_8bit();
}

void st7789_fill_color_dma(uint16_t color) {

  for (uint32_t i = 0; i < 240; i++) {
    line_buffer[i] = color;
  }

  st7789_set_window(0, 0, 239, 239);

  spi1_wait();
  spi1_set_16bit();

  dc_data();

  for (uint32_t y = 0; y < 240; y++) {

    spi1_write_dma(line_buffer, 240);
  }

  spi1_wait();

  spi1_set_8bit();
}

void st7789_init(void) {
  cs_pin = get_gpio_pin(GPIO_PORT_B, 6);
  dc_pin = get_gpio_pin(GPIO_PORT_B, 8);
  rst_pin = get_gpio_pin(GPIO_PORT_B, 7);
  gpio_reset(cs_pin);
  rcc_enable_gpio(GPIO_PORT_B);

  gpio_set_mode(cs_pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(dc_pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(rst_pin, GPIO_OUTPUT_MODE);

  cs_high();

  /*
   * Hardware reset
   */

  rst_high();

  for (volatile uint32_t i = 0; i < 100000; i++)
    ;

  rst_low();

  for (volatile uint32_t i = 0; i < 100000; i++)
    ;

  rst_high();

  for (volatile uint32_t i = 0; i < 100000; i++)
    ;

  /*
   * Sleep out
   */

  st7789_write_command(0x11);

  for (volatile uint32_t i = 0; i < 200000; i++)
    ;

  /*
   * Pixel format
   */

  st7789_write_command(0x3A);
  st7789_write_data(0x05);

  /*
   * Memory access
   */

  st7789_write_command(0x36);
  st7789_write_data(0x00);

  /*
   * Required for many ST7789 panels
   */

  st7789_write_command(0x21);
  st7789_write_command(0x13);

  /*
   * Display ON
   */

  st7789_write_command(0x29);

  for (volatile uint32_t i = 0; i < 100000; i++)
    ;
  st7789_fill_color_dma(0xFF00);
}

static void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             uint16_t color) {

  st7789_set_window(x, y, x + w - 1, y + h - 1);

  spi1_wait();
  spi1_set_16bit();

  dc_data();

  for (uint32_t i = 0; i < w * h; i++) {
    spi1_write_word(color);
  }

  spi1_wait();
  spi1_set_8bit();
}
void fb_flush(void) {

  st7789_set_window(0, 0, 239, 39);

  spi1_wait();
  spi1_set_16bit();

  dc_data();

  spi1_write_dma(framebuffer, 240 * 40);

  spi1_wait();

  spi1_set_8bit();
}

void draw_square(uint16_t x, uint16_t y, uint16_t color) {

  st7789_fill_rect(x, y, 20, 20, color);
}

void draw_moving_square(uint16_t old_x, uint16_t new_x) {

  // очистить буфер
  for (uint32_t i = 0; i < 24 * 20; i++) {
    rect_buffer[i] = 0xFF00;
  }

  // новый квадрат внутри буфера
  for (uint16_t y = 0; y < 20; y++) {

    for (uint16_t x = 0; x < 20; x++) {

      uint16_t px = (new_x - old_x) + x;

      if (px < 24) {
        rect_buffer[y * 24 + px] = 0xF800;
      }
    }
  }

  // обновить только region
  st7789_set_window(old_x, 100, old_x + 23, 119);

  spi1_wait();
  spi1_set_16bit();

  dc_data();

  spi1_write_dma(rect_buffer, 24 * 20);

  spi1_wait();
  spi1_set_8bit();
}

void st7789_demo_tick(void) {
  static uint16_t old_x = 0;
  static uint16_t x = 0;

  x += 1;

  if (x > 216) {
    x = 0;
  }

  draw_moving_square(old_x, x);

  old_x = x;

  for (volatile uint32_t i = 0; i < 15000; i++) {
  }
}
