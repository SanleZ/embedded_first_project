#include "app/button.h"
#include "app/button_manager.h"
#include "app/led.h"
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

#define BTN_1_DEBOUNCE_MS 100
#define BTN_2_DEBOUNCE_MS 50

#define BTN_1_LONG_PRESS_MS 800
#define BTN_2_LONG_PRESS_MS 500

#define BTN_1_DOUBLE_CLICK_MS 300
#define BTN_2_DOUBLE_CLICK_MS 300

#define BTN_1_REPEAT_START_MS 1000
#define BTN_2_REPEAT_START_MS 1000

#define BTN_1_REPEAT_INTERVAL_MS 300
#define BTN_2_REPEAT_INTERVAL_MS 100

static button_t btn1;
static button_t btn2;
static led_t led1;

void on_button_1_irq(void) { button_handle_edge(&btn1); }

void on_button_2_irq(void) { button_handle_edge(&btn2); }

int main(int argc, char *argv[]) {
  event_init();
  gpio_pin_t gpio_led1_pin = get_gpio_pin(GPIO_PORT_C, LED_LINE_13);
  gpio_pin_t gpio_btn1_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_0);
  gpio_pin_t gpio_btn2_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_3);

  button_init(&btn1, gpio_btn1_pin, BTN_1_DEBOUNCE_MS, BUTTON_ACTIVE_LOW,
              BTN_1_LONG_PRESS_MS, BTN_1_DOUBLE_CLICK_MS, BTN_1_REPEAT_START_MS,
              BTN_1_REPEAT_INTERVAL_MS, BUTTON_ID_SWITCH);
  button_init(&btn2, gpio_btn2_pin, BTN_2_DEBOUNCE_MS, BUTTON_ACTIVE_LOW,
              BTN_2_LONG_PRESS_MS, BTN_2_DOUBLE_CLICK_MS, BTN_2_REPEAT_START_MS,
              BTN_2_REPEAT_INTERVAL_MS, BUTTON_ID_CANCEL);

  button_manager_init();
  button_manager_add(&btn1);
  button_manager_add(&btn2);

  led_init(&led1, gpio_led1_pin);

  rcc_enable_gpio(gpio_led1_pin);
  rcc_enable_gpio(gpio_btn1_pin);
  rcc_enable_gpio(gpio_btn2_pin);
  rcc_enable_syscfg();

  gpio_set_mode(gpio_led1_pin, GPIO_OUTPUT_MODE);
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

  event_t e;

  while (1) {
    while ((e = event_pop()).type != EVENT_NONE) {
      switch (e.type) {
      case EVENT_BUTTON_SINGLE_CLICK: {

        led_mode_t led_status = led1.mode == LED_MODE_ALWAYS_ON
                                    ? LED_MODE_BLINK_SLOW
                                    : LED_MODE_ALWAYS_ON;
        led_set_mode(&led1, led_status);
        break;
      }
      case EVENT_BUTTON_LONG_PRESS:
        led_set_mode(&led1, LED_MODE_BLINK_FAST);
        break;
      case EVENT_BUTTON_DOUBLE_CLICK:
        led_set_mode(&led1, LED_MODE_BLINK_SUPER_FAST);
        break;
      case EVENT_BUTTON_REPEAT:
        led_set_mode(&led1, LED_MODE_BLINK_SUPER_SLOW);
        break;
      default:
        break;
      }
    }
    button_manager_update();
    led_update(&led1);
  }
}
