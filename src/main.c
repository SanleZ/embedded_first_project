#include "app/button.h"
#include "common/common.h"
#include "core/event.h"
#include "core/systick.h"
#include "drivers/exti.h"
#include "drivers/gpio.h"
#include "drivers/nvic.h"
#include "drivers/rcc.h"
#include "drivers/syscfg.h"
#include <stdint.h>

volatile uint32_t last_button_irq = 0;
#define BTN_LINE 0
#define LED_LINE 13

void on_button_irq(void) {
  event_push((event_t){.type = EVENT_BUTTON_EDGE, .data = BTN_LINE});
}

int main(int argc, char *argv[]) {
  event_init();
  button_t btn;
  gpio_pin_t led = get_gpio_pin(GPIO_PORT_C, LED_LINE);
  gpio_pin_t gpio_btn_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE);

  button_init(&btn, gpio_btn_pin, 50, BUTTON_ACTIVE_LOW);

  rcc_enable_gpio(led);
  rcc_enable_gpio(gpio_btn_pin);
  rcc_enable_syscfg();

  gpio_set_mode(led, GPIO_OUTPUT_MODE);
  gpio_set_mode(gpio_btn_pin, GPIO_INPUT_MODE);
  gpio_set_pull(gpio_btn_pin, GPIO_PULLUP);

  systick_init(16000, SYSTICK_SRC_AHB);

  /* Map EXTI line to GPIO */
  syscfg_map_exti_line(gpio_btn_pin);

  // EXTI configuration
  exti_configure_line(gpio_btn_pin.pin, EXTI_TRIGGER_FALLING);
  exti_enable_line(gpio_btn_pin.pin);

  // register callback
  exti_register_callback(gpio_btn_pin.pin, on_button_irq);

  // Enable interrrupt in NVIC
  nvic_enable_irq(EXTI0_IRQn);

  volatile uint8_t blink_state = 1;
  volatile uint32_t last_blink_time = 0;
  volatile uint8_t led_on = 1;
  event_t e;
  while (1) {
    while ((e = event_pop()).type != EVENT_NONE) {
      switch (e.type) {
      case EVENT_BUTTON_EDGE:
        button_handle_edge(&btn);
        break;
      case EVENT_BUTTON_PRESS:
        blink_state = !blink_state;
        break;
      default:
        break;
      }
    }
    button_update(&btn);

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
