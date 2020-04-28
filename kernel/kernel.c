#include "exception.h"
#include "ioutil.h"
#include "libsyscall.h"
#include "mini_uart.h"
#include "sched.h"
#include "shell.h"

int main(void) {
  gpio_init();
  mini_uart_init();
  exception_init();
  core_timer_enable();
//  el1_to_el0();
//  shell();

  idle_task_init();
  for (int i = 0; i < 10; ++i) {
//    privilege_task_create(foo);
    privilege_task_create(user_test);
  }

  idle();
}

void idle(void) {
  while (true) {
    printf("CPU0 is in idle state..." EOL);

    uint64_t sp;
    asm("mov %0, sp" : "=r"(sp));
    printf(">> sp = %#x\n", sp);

    for (int i = 0; i < 100000; ++i) {}
  }
}

void foo(void) {
  for (uint64_t i = 0; ; ++i) {
    printf("%u: %u..." EOL, get_current_task()->id, i);

    uint64_t sp;
    asm("mov %0, sp" : "=r"(sp));
    printf(">> sp = %#x\n", sp);

    for (int i = 0; i < 100000; ++i) {}
  }
}

void user_test(void) {
  do_exec(test);
}

void test(void) {
  uint64_t i = 0;
  while (true) {
    for (int i = 0; i < 100000; ++i) {}
    printf("user %u: %u\n", get_taskid(), i++);

    uint64_t sp, fp;
    asm("mov %0, sp" : "=r"(sp));
    asm("mov %0, x29" : "=r"(fp));
    printf("sp = %#x\n", sp);
    printf("fp = %#x\n", fp);
  }
}
