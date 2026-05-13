#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define RING_BUFFER_SIZE 64

typedef struct {
  uint8_t buffer[RING_BUFFER_SIZE];
  volatile uint16_t head;
  volatile uint16_t tail;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb);

bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte);

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *byte);

#endif // !RING_BUFFER_H
