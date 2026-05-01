#include "event.h"
#include <stdint.h>

#define EVENT_QUEUE_SIZE 8

static volatile event_t queue[EVENT_QUEUE_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static volatile uint32_t total_pushed = 0;
static volatile uint32_t total_dropped = 0;

void event_init() {
  head = 0;
  tail = 0;
}

uint8_t event_push(event_t e) {
  uint8_t next = (head + 1) % EVENT_QUEUE_SIZE;

  if (next == tail) {
    total_dropped++;
    return 0;
  }

  queue[head] = e;
  head = next;
  total_pushed++;
  return 1;
}

event_t event_pop() {
  event_t e = {.type = EVENT_NONE};
  if (head == tail) {
    return e;
  }
  e = queue[tail];
  tail = (tail + 1) % EVENT_QUEUE_SIZE;
  return e;
}

uint8_t event_count(void) {
  if (head >= tail) {
    return head - tail;
  }
  return EVENT_QUEUE_SIZE - tail + head;
}

uint32_t event_total_pushed(void) { return total_pushed; }

uint32_t event_total_dropped(void) { return total_dropped; }
