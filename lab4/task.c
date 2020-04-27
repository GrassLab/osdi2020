#include "task.h"
#include "task_switch.h"
#include "uart.h"
#define current get_current();

int task_count = 0;
int runqueue_count = 0;
char kstack_pool[64][4096];

void privilege_task_create(void (*do_func)()) {
  task_pool[task_count].id = task_count;
  task_pool[task_count].func = do_func;
  runqueue[runqueue_count++] = task_count;
  task_count++;
}

void context_switch(task *next) {
  task *prev = current;
  switch_to(prev, next);
}

static int schedule_index = -1;

void schedule() {
  schedule_index = (schedule_index + 1) % task_count;
  int task_id = runqueue[schedule_index];
  task_pool[task_id].func();
}
