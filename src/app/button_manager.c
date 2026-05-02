#include "button_manager.h"
#include "app/button.h"
#include <stdint.h>

#define BUTTONS_NUMBER 10

static button_t *buttons[BUTTONS_NUMBER];
static uint8_t button_count = 0;

void button_manager_init(void) { button_count = 0; }

void button_manager_add(button_t *btn) {
  if (button_count < BUTTONS_NUMBER) {
    buttons[button_count++] = btn;
  }
}

void button_manager_update(void) {
  for (uint8_t i = 0; i < button_count; i++) {
    button_update(buttons[i]);
  }
}
