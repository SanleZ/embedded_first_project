#pragma once

#include <stdint.h>

typedef void (*task_handler_t)(void);

typedef struct {
  uint32_t interval;
  uint32_t last_run;
  task_handler_t handler;
} task_t;

void scheduler_run(task_t *tasks, uint32_t count);
