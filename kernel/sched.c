#include "exception.h"
#include "ioutil.h"
#include "mini_uart.h"
#include "sched.h"
#include "shell.h"
#include "string.h"
#include "syscall.h"

/*
 * Create the permanently exist idle task.
 * It should always occupy task id 0.
 */
void idle_task_create() {
  if (privilege_task_init() != 0) {
    printk("[ERROR] Task id 0 is already used" EOL);
    while (true) {}
  }
  asm volatile("msr tpidr_el1, %0" : : "r"(&task_pool[0]));
}

uint32_t privilege_task_init(void) {
  uint32_t id = 0;
  for (; id < MAX_TASK_NUM && task_inuse[id]; ++id) {}
  if (id >= MAX_TASK_NUM) {
    mini_uart_puts("[ERROR] Can't create more tasks" EOL);
    return 0;
  }

  memset(&task_pool[id], 0, sizeof(struct task));
  task_inuse[id] = true;
  task_pool[id].id = id;
  task_pool[id].timeslice = DEFAULT_TIMESLICE;
  task_pool[id].preempt_count = 0;
  task_pool[id].state = TASK_RUNNING;
  return id;
}

uint32_t privilege_task_create(void(*func)(void)) {
  uint32_t id = privilege_task_init();

  task_pool[id].context.x19 = (uint64_t)func;
  task_pool[id].context.lr = (uint64_t)task_debut_hook;
  /* "+1" because stack grows toward lower address. */
  task_pool[id].context.sp = (uint64_t)kstack_pool[id + 1];
  enqueue(&runqueue, &task_pool[id]);
  return id;
}

void context_switch(struct task *next) {
  struct task *prev = get_current_task();
  if (prev != next) {
    context_switch_helper(prev, next);
  }
}

void schedule(void) {
  if (get_current_task()->preempt_count == 0) {
    enqueue(&runqueue, get_current_task());

    /* Select the next task by simple round-robin policy. */
    struct task *victim = dequeue(&runqueue);
    while (victim->state != TASK_RUNNING) {
      enqueue(&runqueue, victim);
      victim = dequeue(&runqueue);
    }

    printk("Context switch is triggered: %u -> %u" EOL, do_get_taskid(), victim->id);
    get_current_task()->timeslice = DEFAULT_TIMESLICE;
    context_switch(victim);
  }
}

/* This function will be invoked after exception handler return. */
void post_exception_hook(void) {
  struct task *current = get_current_task();
  if (current->timeslice == 0 || current->state == TASK_ZOMBIE) {
    enable_interrupt();
    schedule();
    disable_interrupt();
  }

  bool *sig_pending = current->sig_pending;
  if (sig_pending[SIGKILL]) {
    do_exit(0);
  }
}

void preempt_enable(void) {
  get_current_task()->preempt_count -= 1;
}

void preempt_disable(void) {
  get_current_task()->preempt_count += 1;
}
