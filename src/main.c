#include "app/button.h"
#include "app/button_manager.h"
#include "app/cli.h"
#include "app/encoder.h"
#include "app/led.h"
#include "common/common.h"
#include "common/string_utils.h"
#include "core/event.h"
#include "core/scheduler.h"
#include "core/systick.h"
#include "drivers/exti.h"
#include "drivers/gpio.h"
#include "drivers/nvic.h"
#include "drivers/rcc.h"
#include "drivers/syscfg.h"
#include "drivers/timer.h"
#include "drivers/uart.h"
#include "stm32f411xe.h"
#include <stdint.h>

static uart_t debug_uart;
volatile uint32_t last_button_irq = 0;
#define BTN_LINE_0 0
#define BTN_LINE_2 2
#define BTN_LINE_3 3
#define BTN_LINE_4 4
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
static led_t led2;
static encoder_t encoder1;
static uint16_t brightness = 500;
static const uint16_t MIN_BRIGHTNESS = 0;
static const uint16_t MAX_BRIGHTNESS = 1000;
static const uint16_t BRIGHTNESS_STEP = 50;

void on_button_1_irq(void) { button_handle_edge(&btn1); }

void on_button_2_irq(void) { button_handle_edge(&btn2); }

static void print_to_cli() {
  char ccr_value_buffer[12];
  char *ccr_str = str_u32_to_str(TIM3->CCR3, ccr_value_buffer);
  char led_brightness[30] = "LED brightness: ";
  str_concat(led_brightness, ccr_str, 30);
  str_concat(led_brightness, "\r\n", 30);
  uart_write_string(&debug_uart, led_brightness);
}

static void task_buttons(void) { button_manager_update(); }
static void task_led(void) { led_update(&led1); }
static void task_cli(void) { cli_update(); }
static void task_encoder(void) {
  encoder_event_t event = encoder_update(&encoder1);
  switch (event) {
  case ENCODER_EVENT_CW:
    if ((brightness + BRIGHTNESS_STEP) <= MAX_BRIGHTNESS) {
      brightness += BRIGHTNESS_STEP;
    }
    print_to_cli();
    TIM3->CCR3 = brightness;
    break;
  case ENCODER_EVENT_CCW:
    if ((brightness - BRIGHTNESS_STEP) >= MIN_BRIGHTNESS) {
      brightness -= BRIGHTNESS_STEP;
    }
    print_to_cli();
    TIM3->CCR3 = brightness;
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[]) {
  uart_init(&debug_uart, USART2, 115200);
  uart_write_string(&debug_uart, "System boot\r\n");
  event_init();
  gpio_pin_t gpio_led1_pin = get_gpio_pin(GPIO_PORT_C, LED_LINE_13);
  gpio_pin_t gpio_led2_pin = get_gpio_pin(GPIO_PORT_B, 0);
  gpio_pin_t gpio_btn1_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_0);
  gpio_pin_t gpio_btn2_pin = get_gpio_pin(GPIO_PORT_A, BTN_LINE_4);
  gpio_pin_t gpio_enc1_pin_a = get_gpio_pin(GPIO_PORT_B, 3);
  gpio_pin_t gpio_enc1_pin_b = get_gpio_pin(GPIO_PORT_B, 4);

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
  led_init(&led2, gpio_led2_pin);

  encoder_init(&encoder1, gpio_enc1_pin_a, gpio_enc1_pin_b);

  rcc_enable_gpio(gpio_led1_pin.port);
  rcc_enable_gpio(gpio_led2_pin.port);
  rcc_enable_gpio(gpio_btn1_pin.port);
  rcc_enable_gpio(gpio_btn2_pin.port);
  rcc_enable_gpio(gpio_enc1_pin_a.port);
  rcc_enable_gpio(gpio_enc1_pin_b.port);

  rcc_enable_syscfg();

  gpio_set_mode(gpio_led1_pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(gpio_led2_pin, GPIO_AF_MODE);
  gpio_set_alternate_function(gpio_led2_pin, GPIO_AF2);

  gpio_set_mode(gpio_btn1_pin, GPIO_INPUT_MODE);
  gpio_set_mode(gpio_btn2_pin, GPIO_INPUT_MODE);
  gpio_set_pull(gpio_btn1_pin, GPIO_PULLUP);
  gpio_set_pull(gpio_btn2_pin, GPIO_PULLUP);

  gpio_set_mode(gpio_enc1_pin_a, GPIO_INPUT_MODE);
  gpio_set_mode(gpio_enc1_pin_b, GPIO_INPUT_MODE);

  gpio_set_pull(gpio_enc1_pin_a, GPIO_PULLUP);
  gpio_set_pull(gpio_enc1_pin_b, GPIO_PULLUP);

  systick_init(16000, SYSTICK_SRC_AHB);
  timer2_init();
  timer3_pwm_init();
  cli_init(&debug_uart, &led1);

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
  nvic_enable_irq(EXTI4_IRQn);
  nvic_enable_irq(USART2_IRQn);
  nvic_enable_irq(TIM2_IRQn);
  nvic_enable_irq(TIM3_IRQn);

  event_t e;

  task_t tasks[] = {{.interval = 1, .last_run = 0, .handler = task_cli},
                    {.interval = 1, .last_run = 0, .handler = task_buttons},
                    {.interval = 1, .last_run = 0, .handler = task_led},
                    {.interval = 1, .last_run = 0, .handler = task_encoder}};
  while (1) {
    while ((e = event_pop()).type != EVENT_NONE) {
      switch (e.type) {
      case EVENT_TIMER_TICK:
        // led_toggle(&led1);
        break;
      case EVENT_BUTTON_SINGLE_CLICK: {
        led_mode_t led_status =
            led1.mode == LED_MODE_ON ? LED_MODE_OFF : LED_MODE_ON;
        led_set_mode(&led1, led_status);
        break;
      }
      case EVENT_BUTTON_LONG_PRESS:
        led1.blink_period_ms = 300;
        led_set_mode(&led1, LED_MODE_BLINK);
        break;
      case EVENT_BUTTON_DOUBLE_CLICK:
        led1.blink_period_ms = 100;
        led_set_mode(&led1, LED_MODE_BLINK);
        break;
      case EVENT_BUTTON_REPEAT:
        led1.blink_period_ms = 1000;
        led_set_mode(&led1, LED_MODE_BLINK);
        break;
      default:
        break;
      }
    }

    scheduler_run(tasks, sizeof(tasks) / sizeof(tasks[0]));
  }
}
