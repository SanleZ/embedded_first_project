#include "common/common.h"
#include "core/systick.h"
#include "drivers/exti.h"
#include "drivers/gpio.h"
#include "drivers/nvic.h"
#include "drivers/rcc.h"
#include "drivers/syscfg.h"
#include <stdint.h>

volatile uint8_t button_event = 0;

volatile uint32_t last_button_irq = 0;

void on_button_press(void) {
  if ((ticks - last_button_irq) > 50) {
    last_button_irq = ticks;
    button_event = 1;
  }
}

#define BTN_LINE 0
#define LED_LINE 13

int main(int argc, char *argv[]) {
  gpio_pin_t led = get_gpio_pin(GPIO_PORT_C, LED_LINE);
  gpio_pin_t btn = get_gpio_pin(GPIO_PORT_A, BTN_LINE);

  rcc_enable_gpio(led);
  rcc_enable_gpio(btn);
  rcc_enable_syscfg();

  gpio_set_mode(led, GPIO_OUTPUT_MODE);
  gpio_set_mode(btn, GPIO_INPUT_MODE);
  gpio_set_pull(btn, GPIO_PULLUP);

  systick_init(16000, SYSTICK_SRC_AHB);

  /* Map EXTI line to GPIO */
  syscfg_map_exti_line(btn);

  // EXTI configuration
  exti_configure_line(btn.pin, EXTI_TRIGGER_FALLING);
  exti_enable_line(btn.pin);

  // register callback
  exti_register_callback(btn.pin, on_button_press);

  // Enable interrrupt in NVIC
  nvic_enable_irq(EXTI0_IRQn);
  volatile uint8_t button_pressed = 0;
  volatile uint8_t blink_state = 1;
  volatile uint32_t last_blink_time = 0;
  volatile uint32_t last_button_time = 0;
  volatile uint8_t led_on = 1;
  volatile uint8_t button_gpio_status = 1;

  while (1) {

    if (button_event) {
      button_event = 0;
      if (!button_pressed) {
        button_pressed = 1;
        button_gpio_status = gpio_read(btn);
        last_button_time = ticks + 50;
      }
    }

    if (button_pressed && (ticks >= last_button_time)) {
      button_pressed = 0;
      if (button_gpio_status == 0) {
        button_gpio_status = 1;
        blink_state = !blink_state;
      }
    }

    // LED control
    if (blink_state) {
      if ((ticks - last_blink_time) > 300) {
        last_blink_time = ticks;
        gpio_write(led, led_on);
        led_on = !led_on;
      }
    } else {
      gpio_reset(led);
    }
  }
}
