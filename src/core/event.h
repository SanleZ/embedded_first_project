#pragma once
#include <stdint.h>

typedef enum {
  EVENT_NONE = 0,
  EVENT_BUTTON_EDGE,
  EVENT_BUTTON_PRESS,
  EVENT_BUTTON_SINGLE_CLICK,
  EVENT_BUTTON_LONG_PRESS,
  EVENT_BUTTON_DOUBLE_CLICK,
  EVENT_BUTTON_REPEAT,
} event_type_t;

typedef struct {
  event_type_t type;
  uint32_t data;
} event_t;

void event_init(void);
uint8_t event_push(event_t e);
event_t event_pop(void);
uint8_t event_count(void);
uint32_t event_total_pushed(void);
uint32_t event_total_dropped(void);
