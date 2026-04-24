#include "common/common.h"
#include "core/systick.h"
#include "drivers/exti.h"
#include "drivers/gpio.h"
#include "drivers/nvic.h"
#include "drivers/rcc.h"
#include "drivers/syscfg.h"
#include "interrupts.h"
#include <stdint.h>

int main(int argc, char *argv[]) {
  gpio_pin_t led = get_gpio_pin('C', 13);
  gpio_pin_t btn = get_gpio_pin('A', 0);

  rcc_enable_gpio(led);
  rcc_enable_gpio(btn);

  gpio_set_mode(led, GPIO_OUTPUT_MODE);
  gpio_set_mode(btn, GPIO_INPUT_MODE);
  gpio_set_pull(btn, GPIO_PULLUP);

  systick_init(16000, SYSTICK_SRC_AHB);

  rcc_enable_syscfg();
  syscfg_map_exti_line(btn);

  exti_configure_line(btn, EXTI_TRIGGER_FALLING);
  exti_enable_line(btn);

  NVIC_ISER0 |= BIT_OFFSET(6);

  volatile uint32_t last_button_check = 0;
  volatile uint8_t blink_state = 1;
  volatile uint32_t last_blink = 0;

  volatile uint8_t led_on = 1;

  while (1) {
    // Handle button press in main loop (debounced)
    if (button_pressed_flag) {
      button_pressed_flag = 0;
      if ((ticks - last_button_check) > 50) {
        last_button_check = ticks;
        blink_state = !blink_state;
      }
    }

    // LED control
    if (blink_state) {
      if ((ticks - last_blink) > 300) {
        last_blink = ticks;
        gpio_write(led, led_on);
        led_on = !led_on;
      }
    } else {
      gpio_reset(led);
    }
  }
}
