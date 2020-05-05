#include "schedule.h"
#include "string.h"
#include "task.h"
#include "timer.h"
#include "uart.h"
int runtaskcount;
static task_t init_task = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           0, /* state etc */
                           0,
                           0,
                           0};

task_t *task_pool[64] = {
    &(init_task),
};
int usr_first = 0;
user_task_context user_pool[64];

void init() {
  while (1) {
    schedule();
  }
}

static int task_count = 0;
static char kstack_pool[64][THREAD_SIZE] = {0};
static char ks[64][THREAD_SIZE] = {0};
static char ustack_pool[64][THREAD_SIZE] = {0};

int privilege_task_create(unsigned long func, int usr) {
  int task_id = task_count;
  task_t *p = (task_t *)&kstack_pool[task_id][0];
  task_pool[task_id] = p;

  if (!p)
    return -1;
  p->state = TASK_RUNNING;
  p->counter = 2;

  if (usr) {
    p->is_usr = 1;
    user_pool[task_id].spsr = func;
    p->cpu_context.x19 = (unsigned long)&jmp_to_usr;
  } else {
    p->cpu_context.x19 = func;
  }

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)&ks[task_id][THREAD_SIZE];

  task_count++;
  runtaskcount++;
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
  if (current->counter > 0) {
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
  printf("runforkfun = %x %x\n", user_pool[now].spsr, &ustack_pool[now]);
  asm volatile("msr sp_el0, %0" ::"r"(ustack_pool[now]) :);
  asm volatile("msr spsr_el1, %0" ::"r"(0) :);
  asm volatile("msr elr_el1, %0" ::"r"(user_pool[now].spsr) :);
  if (usr_first == 0) {
    usr_first = 1;
    ret_to_usr();
  } else {
    ret_to_fusr();
  }
}

void do_exec(unsigned long fun) {
  privilege_task_create(fun, 1);
  do_exit();
};

void exec(unsigned long rfun) {
  unsigned long rf = rfun;
  printf("exec = %x\n", rf);
  asm volatile("mov x0, #2");
  asm volatile("mov x1, %0" ::"r"(rf) :);
  asm volatile("svc #0");
}

int fork() {
  asm volatile("mov x0, #7");
  asm volatile("svc #0");
  return 0;
}

int do_fork() {
  unsigned long fun;
  asm volatile("MRS %[result], elr_el1" : [result] "=r"(fun));
  printf("fork = %x\n", fun);

  int task_id = task_count;

  int nowid = get_current();

  memcpy((unsigned short *)ks[task_id], (unsigned short *)ks[nowid],
         THREAD_SIZE);
  memcpy((unsigned short *)ustack_pool[task_id],
         (unsigned short *)ustack_pool[nowid], THREAD_SIZE);
  task_t *p = (task_t *)&kstack_pool[task_id][0];
  task_pool[task_id] = p;

  if (!p) {
    printf("error");
    return -1;
  }

  p->state = TASK_RUNNING;
  p->counter = 2;
  p->is_usr = 1;
  user_pool[task_id].spsr = fun;
  p->cpu_context.x19 = (unsigned long)&jmp_to_usr;
  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)&ks[task_id][THREAD_SIZE];

  task_count++;
  runtaskcount++;
  return task_id;
}

void do_exit() {
  task_t *now = task_pool[get_current()];
  now->state = TASK_IDLE;
  runtaskcount--;
  schedule();
}
void exit(int i) {
  asm volatile("mov x0, #8");
  asm volatile("svc #0");
}

void foo1() {
  while (1) {
    printf("Task id: %d\n", get_current());
    wait_cycles(10000000);
    int now = get_current();
    task_t *current = task_pool[now];
    if (current->switchflag == 1) {
      printf("user context switch\n");
      current->switchflag = 0;
      schedule();
    }
    // schedule();
  }
}

void idle1() {
  while (1) {
    schedule();
    wait_cycles(10000000);
  }
}

void test1() {
  task_init();
  for (int i = 0; i < 4; ++i) { // N should > 2
    privilege_task_create((unsigned long)&foo1, 0);
  }
  privilege_task_create((unsigned long)&idle1, 0);
  core_timer_enable();
  schedule();
}

void foo() {
  int tmp = 5;
  printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_current(),
         &tmp, tmp);
  exit(0);
}

void tt() {
  while (1) {
    printf("tt\n");
    wait_cycles(10000000);
  }
}

void ut1() {
  while (1) {
    int cnt = 1;
    printf("test1 %x \n", &cnt);
    wait_cycles(100000000);
  }
}
void ut2() {
  while (1) {
    int cnt = 1;
    printf("test2 %x \n", &cnt);
    wait_cycles(100000000);
  }
}
void user_test1() { do_exec((unsigned long)&ut1); }
void user_test2() { do_exec((unsigned long)&ut2); }
void idle() {
  while (1) {
    if (runtaskcount == 0) {
      break;
    }
    schedule();
    wait_cycles(10000000);
  }
  printf("Test finished\n");
  while (1)
    ;
}

void test2() {
  task_init();
  privilege_task_create((unsigned long)&idle, 0);
  privilege_task_create((unsigned long)&user_test1, 0);
  privilege_task_create((unsigned long)&user_test2, 0);
  core_timer_enable();
  schedule();
}

void test() {
  // int cnt = 1;
  printf("Task\n");
  wait_cycles(10000000);
  fork();
  printf("endfork\n");
  exec((unsigned long)&tt);
}

void user_test() { do_exec((unsigned long)&test); }

void test3() {
  task_init();
  privilege_task_create((unsigned long)&idle, 0);
  privilege_task_create((unsigned long)&user_test, 0);
  core_timer_enable();
  schedule();
}
