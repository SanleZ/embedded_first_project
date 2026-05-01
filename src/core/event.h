#pragma once
#include <stdint.h>

typedef enum {
  EVENT_NONE = 0,
  EVENT_BUTTON_PRESSED,
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
