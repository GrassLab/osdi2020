#include "schedule.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

unsigned long execfun;
static task_t init_task = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           0, /* state etc */
                           0,
                           1,
                           0,
                           0,
                           0};

task_t *task_pool[64] = {
    &(init_task),
};

user_task_context user_pool[64];

void init() {
  while (1) {
    schedule();
  }
}

static int task_count = 0;
static unsigned short kstack_pool[64][THREAD_SIZE] = {0};
static unsigned short ustack_pool[64][THREAD_SIZE] = {0};

int privilege_task_create(unsigned long func, int usr) {
  int task_id = task_count;
  task_t *p = (task_t *)&kstack_pool[task_id][0];
  task_pool[task_id] = p;

  if (!p)
    return -1;
  p->priority = 1;
  p->state = TASK_RUNNING;
  p->counter = 2;
  p->preempt_count = 0; // disable preemtion until schedule_tail

  if (usr) {
    p->is_usr = 1;
    user_pool[task_id].spsr = func;
    p->cpu_context.x19 = (unsigned long)&jmp_to_usr;
  } else {
    p->cpu_context.x19 = func;
  }

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

  task_count++;
  return task_id;
}

void task_init() {
  int task_id = privilege_task_create((unsigned long)init, 0);
  task_pool[task_id]->state = TASK_IDLE;
  set_current(task_id);
}

void context_switch(int task_id) {
  int prev_id = get_current();
  set_current(task_id);
  task_t *now = task_pool[task_id];
  if (now->is_usr) {
    now->cpu_context.pc = (unsigned long)&jmp_to_usr;
  }
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
  current->switchflag = 1;
  if (current->is_usr) {
    jmp_to_usr();
  }
}

void jmp_to_usr() {
  int now = get_current();
  task_t *current = task_pool[now];
  if (current->switchflag == 1) {
    printf("user context switch\n");
    current->switchflag = 0;
    schedule();
  }
  printf("runfun = %x\n", user_pool[now].spsr);
  asm volatile("msr sp_el0, %0" ::"r"(ustack_pool[now]) :);
  asm volatile("msr spsr_el1, %0" ::"r"(0) :);
  asm volatile("msr elr_el1, %0" ::"r"(user_pool[now].spsr) :);
  asm volatile("eret");
}

void t1() {
  while (1) {
    uart_puts("1...\n");
    wait_cycles(10000);
    schedule();
  }
}

void t2() {
  while (1) {
    uart_puts("2...\n");
    wait_cycles(10000);
    schedule();
  }
}
void i1() {
  while (1) {
    uart_puts("1...\n");
    wait_cycles(1000000);
    task_t *current = task_pool[get_current()];
    if (current->switchflag == 1) {
      current->switchflag = 0;
      schedule();
    }
  }
}

void i2() {
  while (1) {
    uart_puts("2...\n");
    wait_cycles(1000000);
    task_t *current = task_pool[get_current()];
    if (current->switchflag == 1) {
      current->switchflag = 0;
      schedule();
    }
  }
}

void scs() {
  task_init();
  privilege_task_create((unsigned long)&t1, 0);
  privilege_task_create((unsigned long)&t2, 0);
  schedule();
}

void ics() {
  task_init();
  privilege_task_create((unsigned long)&i1, 0);
  privilege_task_create((unsigned long)&i2, 0);
  asm volatile("msr DAIFClr, 0xf;");
  core_timer_enable();
  schedule();
}

void u1() {
  while (1) {
    uart_puts("u1...\n");
    wait_cycles(10000000);
  }
}

void u2() {
  while (1) {
    uart_puts("u2...\n");
    wait_cycles(10000000);
  }
}

void do_exec() {
  task_init();
  privilege_task_create(execfun, 1);
  schedule();
};

void exec(unsigned long fun) {
  execfun = fun;
  asm volatile("mov x0, #5");
  asm volatile("svc #0");
}

void do_ucs() {
  task_init();
  privilege_task_create((unsigned long)&u1, 1);
  privilege_task_create((unsigned long)&u2, 1);
  asm volatile("msr DAIFClr, 0xf;");
  core_timer_enable();
  schedule();
}

void ucs() {
  asm volatile("mov x0, #6");
  asm volatile("svc #0");
}