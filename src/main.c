#include "app/button.h"
#include "app/button_manager.h"
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
#define BTN_LINE_0 0
#define BTN_LINE_3 3
#define LED_LINE_13 13

typedef enum {
  LED_BLINK_FAST,
  LED_BLINK_SLOW,
  LED_CONTINUOUS_LIGHTNING,
  LED_BLINK_SUPER_FAST,
  LED_BLINK_SUPER_SLOW

} led_lightning_t;

static button_t btn1;
static button_t btn2;

void on_button_1_irq(void) { button_handle_edge(&btn1); }

void on_button_2_irq(void) { button_handle_edge(&btn2); }

int main(int argc, char *argv[]) {
  event_init();
  gpio_pin_t led = get_gpio_pin(GPIO_PORT_C, LED_LINE_13);
  gpio_pin_t gpio_btn1_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_0);
  gpio_pin_t gpio_btn2_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_3);

  button_init(&btn1, gpio_btn1_pin, 100, BUTTON_ACTIVE_LOW, 800, 300, 1000, 300,
              BUTTON_ID_SWITCH);
  button_init(&btn2, gpio_btn2_pin, 50, BUTTON_ACTIVE_LOW, 500, 300, 1000, 100,
              BUTTON_ID_CANCEL);

  button_manager_init();
  button_manager_add(&btn1);
  button_manager_add(&btn2);

  rcc_enable_gpio(led);
  rcc_enable_gpio(gpio_btn1_pin);
  rcc_enable_gpio(gpio_btn2_pin);
  rcc_enable_syscfg();

  gpio_set_mode(led, GPIO_OUTPUT_MODE);
  gpio_set_mode(gpio_btn1_pin, GPIO_INPUT_MODE);
  gpio_set_mode(gpio_btn2_pin, GPIO_INPUT_MODE);
  gpio_set_pull(gpio_btn1_pin, GPIO_PULLUP);
  gpio_set_pull(gpio_btn2_pin, GPIO_PULLUP);

  systick_init(16000, SYSTICK_SRC_AHB);

  /* Map EXTI line to GPIO */
  syscfg_map_exti_line(gpio_btn1_pin);
  syscfg_map_exti_line(gpio_btn2_pin);

  // EXTI configuration
  exti_configure_line(gpio_btn1_pin.pin, EXTI_TRIGGER_FALLING);
  exti_enable_line(gpio_btn1_pin.pin);

  exti_configure_line(gpio_btn2_pin.pin, EXTI_TRIGGER_FALLING);
  exti_enable_line(gpio_btn2_pin.pin);

  // register callback
  exti_register_callback(gpio_btn1_pin.pin, on_button_1_irq);
  exti_register_callback(gpio_btn2_pin.pin, on_button_2_irq);

  // Enable interrrupt in NVIC
  nvic_enable_irq(EXTI0_IRQn);
  nvic_enable_irq(EXTI3_IRQn);

  volatile uint32_t last_blink_time = 0;
  volatile led_lightning_t led_status = LED_BLINK_SLOW;
  volatile uint8_t led_on = 1;
  event_t e;

  while (1) {
    while ((e = event_pop()).type != EVENT_NONE) {
      switch (e.type) {
      case EVENT_BUTTON_SINGLE_CLICK:
        led_status = led_status == LED_BLINK_SLOW ? LED_CONTINUOUS_LIGHTNING
                                                  : LED_BLINK_SLOW;
        break;
      case EVENT_BUTTON_LONG_PRESS:
        led_status = LED_BLINK_FAST;
        break;
      case EVENT_BUTTON_DOUBLE_CLICK:
        led_status = LED_BLINK_SUPER_FAST;
        break;
      case EVENT_BUTTON_REPEAT:
        led_status = LED_BLINK_SUPER_SLOW;
        break;
      default:
        break;
      }
    }
    button_manager_update();

    // LED control
    switch (led_status) {
    case LED_CONTINUOUS_LIGHTNING:
      gpio_reset(led);
      break;
    case LED_BLINK_SLOW:
    case LED_BLINK_FAST:
      if ((ticks - last_blink_time) >
          (led_status == LED_BLINK_SLOW ? 300 : 100)) {
        last_blink_time = ticks;
        gpio_write(led, led_on);
        led_on = !led_on;
      }
      break;
    case LED_BLINK_SUPER_FAST:
      if ((ticks - last_blink_time) > 50) {
        last_blink_time = ticks;
        gpio_write(led, led_on);
        led_on = !led_on;
      }
      break;
    case LED_BLINK_SUPER_SLOW:
      if ((ticks - last_blink_time) > 1000) {
        last_blink_time = ticks;
        gpio_write(led, led_on);
        led_on = !led_on;
      }
      break;
    default:
      break;
    }
  }
}
