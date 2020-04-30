#include "exception.h"
#include "ioutil.h"
#include "libsyscall.h"
#include "mini_uart.h"
#include "sched.h"
#include "shell.h"
#include "syscall.h"

int main(void) {
  gpio_init();
  mini_uart_init();
  exception_init();
  core_timer_enable();
//  el1_to_el0();
//  shell();

  idle_task_init();
  for (int i = 0; i < 1; ++i) {
    //privilege_task_create(foo);
    privilege_task_create(user_test);
  }

  idle();
}

void idle(void) {
  while (true) {
//    printf("CPU0 is in idle state..." EOL);
//
//    uint64_t sp;
//    asm("mov %0, sp" : "=r"(sp));
//    printf(">> sp = %#x\n", sp);
//
//    for (int i = 0; i < 100000; ++i) {}
    schedule();
  }
}

void foo(void) {
  for (uint64_t i = 0; ; ++i) {
    printf("%u: %u..." EOL, get_current_task()->id, i);

    uint64_t sp;
    asm("mov %0, sp" : "=r"(sp));
    printf(">> sp = %#x\n", sp);

    for (int i = 0; i < 10000000; ++i) {}
//    printf("Task %u is reading..." EOL, get_current_task()->id);
//    preempt_disable();
//    for (;;) {
//      char c = mini_uart_getc(true);
//      if (c == 'q') {
//        break;
//      }
//    }
//    preempt_enable();
//    schedule();
  }
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

void user_test(void) {
  do_exec(test);
  //do_exec(exec_test);
}

void test(void) {
//  uint64_t i = 0;
//  while (true) {
//    for (int i = 0; i < 100000000; ++i) {}
//    uint64_t sp;
//    asm("mov %0, sp" : "=r"(sp));
//    printf("user %u: SP_EL0 is %#x, %u\n", get_taskid(), sp, i++);
//
//    uint64_t sp, fp;
//    asm("mov %0, sp" : "=r"(sp));
//    asm("mov %0, x29" : "=r"(fp));
//    printf("sp = %#x\n", sp);
//    printf("fp = %#x\n", fp);
//    char buf[32] = "";
//    printf("User %u is reading..." EOL, get_taskid());
//    uart_read(buf, 4);
//    printf("User %u read: \"", get_taskid());
//    uart_write(buf, strlen(buf));
//    printf("\"" EOL EOL);
//  }

//  int cnt = 1;
//  if (fork() == 0) {
//    fork();
//    delay(100000);
//    fork();
//    while(cnt < 10) {
//      printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
//      delay(100000);
//      ++cnt;
//    }
//    exit(0);
//    printf("Should not be printed\n");
//  } else {
//    printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
//    exec(foo);
//  }

  uint64_t sp;
  asm("mov %0, sp" : "=r"(sp));
  printf("Task %u: SP_EL0: %#x\n", get_taskid(), sp);
  int a = 3;
  int id = fork();
  if (id == 0) {
    printf("Task %u: This is the child\n", get_taskid());
    a = 4;

    asm("mov %0, sp" : "=r"(sp));
    for (;;) {
      for (int i = 0; i < 10000000; ++i) {}
      printf("Task %u: stack = %#x, a = %u\n", get_taskid(), sp, a);
    }
  } else {
    printf("Task %u: This is the PARENT\n", get_taskid());

    asm("mov %0, sp" : "=r"(sp));
    for (;;) {
      for (int i = 0; i < 10000000; ++i) {}
      printf("Task %u: stack = %#x, a = %u\n", get_taskid(), sp, a);
    }
  }
}
