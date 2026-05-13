#include "scheduler.h"
#include "drivers/timer.h"
#include <stdint.h>

void scheduler_run(task_t *tasks, uint32_t count) {
  uint32_t now = timer_get_ticks();

  for (uint32_t i = 0; i < count; i++) {
    if ((now - tasks[i].last_run) > tasks[i].interval) {
      tasks[i].last_run = now;
      tasks[i].handler();
    }
  }
}
