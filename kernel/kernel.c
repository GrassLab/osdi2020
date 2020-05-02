#include "exception.h"
#include "ioutil.h"
#include "libsyscall.h"
#include "mini_uart.h"
#include "sched.h"
#include "shell.h"
#include "syscall.h"

void delay(int t) {
  for (int i = 0; i < t; ++i) {}
}

size_t num_runnable_tasks(void) {
  size_t num = 0;
  for (int i = 0; i < MAX_TASK_NUM; ++i) {
    if (task_inuse[i]) {
      ++num;
    }
  }
  return num;
}

void reaper(void) {
  while (true) {
    for (int i = 0; i < MAX_TASK_NUM; ++i) {
      if (task_inuse[i] == true && task_pool[i].state == TASK_ZOMBIE) {
        task_inuse[i] = false;
        printf("Task %u is reaped\n", i);
      }
    }
  }
}

void idle(void) {
  while (true) {
    if (num_runnable_tasks() == 1) {
      break;
    }
    schedule();
    delay(1000000);
  }
  printf("Test finished\n");
  while (true) {}
}

void exec_success(void) {
  while (true) {
    for (int i = 0; i < 1000000; ++i) {}
    printf("exec syscall works!" EOL);
  }
}

void exec_test(void) {
  exec(exec_success);
  printf("THIS LINE SHOULD NOT BE PRINTED!" EOL);
}

void foo(){
  int tmp = 5;
  printf("Task %u after exec, tmp address 0x%x, tmp value %u\n", get_taskid(), &tmp, tmp);
  exit(0);
}

void test(void) {
//  uint64_t sp;
//  asm("mov %0, sp" : "=r"(sp));
//  printf("Task %u: SP_EL0: %#x\n", get_taskid(), sp);
//  int a = 3;
//  int id = fork();
//  if (id == 0) {
//    printf("Task %u: This is the child\n", get_taskid());
//    a = 4;
//
//    asm("mov %0, sp" : "=r"(sp));
//    for (int c = 0; c < 10; ++c) {
//      for (int i = 0; i < 100000; ++i) {}
//      printf("Task %u: stack = %#x, a = %u\n", get_taskid(), sp, a);
//    }
//    exit(2);
//  } else {
//    printf("Task %u: This is the PARENT\n", get_taskid());
//
//    asm("mov %0, sp" : "=r"(sp));
//    for (int c = 0; c < 10; ++c) {
//      for (int i = 0; i < 100000; ++i) {}
//      printf("Task %u: stack = %#x, a = %u\n", get_taskid(), sp, a);
//    }
//    exit(1);
//  }
  int cnt = 1;
  if (fork() == 0) {
    fork();
    delay(100000);
    fork();
    while(cnt < 10) {
      printf("Task id: %u, cnt: %u\n", get_taskid(), cnt);
      delay(100000);
      ++cnt;
    }
    exit(0);
    printf("Should not be printed\n");
  } else {
    printf("Task %u before exec, cnt address 0x%x, cnt value %u\n", get_taskid(), &cnt, cnt);
    exec(foo);
  }
}

void user_test(void) {
  do_exec(test);
  //do_exec(exec_test);
}

int main(void) {
  gpio_init();
  mini_uart_init();
  exception_init();
  core_timer_enable();
//  el1_to_el0();
//  shell();

  idle_task_init();
  privilege_task_create(reaper);
  for (int i = 0; i < 1; ++i) {
    //privilege_task_create(foo);
    privilege_task_create(user_test);
  }

  idle();
}
