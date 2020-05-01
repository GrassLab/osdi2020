#include "ioutil.h"
#include "mini_uart.h"
#include "sched.h"
#include "shell.h"

/*
 * Create the permanently exist idle task.
 * It should always occupy task id 0.
 */
void idle_task_init() {
  task_inuse[0] = true;
  task_pool[0].id = 0;
  task_pool[0].timeslice = DEFAULT_TIMESLICE;
  task_pool[0].preempt_count = 0;
  task_pool[0].state = TASK_RUNNING;
  asm volatile("msr tpidr_el1, %0" : : "r"(&task_pool[0]));
}

uint32_t privilege_task_init(void) {
  uint32_t id = 1;
  for (; id < MAX_TASK_NUM && task_inuse[id]; ++id) {}
  if (id >= MAX_TASK_NUM) {
    mini_uart_puts("[ERROR] Can't create more tasks" EOL);
    return 0;
  }

  task_inuse[id] = true;
  task_pool[id].id = id;
  task_pool[id].timeslice = DEFAULT_TIMESLICE;
  task_pool[id].preempt_count = 0;
  task_pool[id].state = TASK_RUNNING;
  return id;
}

void privilege_task_create(void(*func)(void)) {
  uint32_t id = privilege_task_init();

  task_pool[id].context.x19 = (uint64_t)func;
  task_pool[id].context.lr = (uint64_t)task_debut_hook;
  /* "+1" because stack grows toward lower address. */
  //task_pool[id].context.fp = (uint64_t)kstack_pool[id + 1];
  task_pool[id].context.sp = (uint64_t)kstack_pool[id + 1];
  enqueue(&runqueue, &task_pool[id]);
}

void context_switch(struct task *next) {
  struct task *prev = get_current_task();
  context_switch_helper(prev, next);
  uint64_t sp;
  asm("mov %0, sp" : "=r"(sp));
  printf("Task %u: SP_EL1: %#x, After context switch\n", do_get_taskid(), sp);
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
    context_switch(victim);
  }
}

/* This function will be invoked after exception handler return. */
void post_exception_hook(void) {
  if ((get_current_task()->timeslice == 0 || get_current_task()->state == TASK_ZOMBIE) && get_current_task()->preempt_count == 0) {
    printf("Task %u: Context switch is triggered" EOL, do_get_taskid());
    get_current_task()->timeslice = DEFAULT_TIMESLICE;
    schedule();
  }
}

void preempt_enable(void) {
  get_current_task()->preempt_count -= 1;
}

void preempt_disable(void) {
  get_current_task()->preempt_count += 1;
}
