#include "task.h"
#include "timer.h"
#include "uart.h"
#define TASK_IDLE 0
#define TASK_RUNNING 1
#define THREAD_SIZE 4096

typedef struct cpu_context_t {
  unsigned long x19;
  unsigned long x20;
  unsigned long x21;
  unsigned long x22;
  unsigned long x23;
  unsigned long x24;
  unsigned long x25;
  unsigned long x26;
  unsigned long x27;
  unsigned long x28;
  unsigned long fp;
  unsigned long sp;
  unsigned long pc;
} cpu_context_t;

typedef struct task_t {
  cpu_context_t cpu_context;
  long state;
  long counter;
  long priority;
  long preempt_count;
} task_t;

static task_t init_task = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           0, /* state etc */
                           0,
                           1,
                           0};

task_t *task_pool[64] = {
    &(init_task),
};

void schedule();
void init() {
  while (1) {
    schedule();
  }
}

static int task_count = 0;
static unsigned short kstack_pool[64][THREAD_SIZE] = {0};

int privilege_task_create(unsigned long func) {
  int task_id = task_count;
  task_t *p = (task_t *)&kstack_pool[task_id][0];
  task_pool[task_id] = p;

  if (!p)
    return -1;
  p->priority = 1;
  p->state = TASK_RUNNING;
  p->counter = 2;
  p->preempt_count = 0; // disable preemtion until schedule_tail

  p->cpu_context.x19 = func;
  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

  task_count++;
  return task_id;
}

void task_init() {
  int task_id = privilege_task_create((unsigned long)init);
  task_pool[task_id]->state = TASK_IDLE;
  set_current(task_id);
}

void context_switch(int task_id) {
  int prev_id = get_current();
  set_current(task_id);
  switch_to(task_pool[prev_id], task_pool[task_id]);
}

static int schedule_index = 0;

void schedule() {
  int task_id;
  task_t *p;
  while (1) {
    task_id = (++schedule_index) % task_count;
    p = task_pool[task_id];
    if (p && p->state == TASK_RUNNING) {
      break;
    }
  }
  context_switch(task_id);
}

void timer_tick() {
  task_t *current = task_pool[get_current()];
  printf("counter = %d\n", current->counter);
  --current->counter;
  if (current->counter > 0 || current->preempt_count > 0) {
    printf("nono\n");
    return;
  }
  printf("switch\n");
  current->counter = 2;
  schedule();
}

void t1() {
  while (1) {
    uart_puts("1...\n");
    wait_cycles(1000000);
    schedule();
  }
}

void t2() {
  while (1) {
    uart_puts("2...\n");
    wait_cycles(1000000);
    schedule();
  }
}
void i1() {
  while (1) {
    uart_puts("1...\n");
    wait_cycles(1000000000);
  }
}

void i2() {
  while (1) {
    uart_puts("2...\n");
    wait_cycles(1000000000);
  }
}

void scs() {
  task_init();
  privilege_task_create((unsigned long)&t1);
  privilege_task_create((unsigned long)&t2);
  schedule();
}

void ics() {
  task_init();
  privilege_task_create((unsigned long)&i1);
  privilege_task_create((unsigned long)&i2);
  asm volatile("msr DAIFClr, 0xf;");
  core_timer_enable();
  schedule();
}