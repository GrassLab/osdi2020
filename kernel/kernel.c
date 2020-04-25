#include "exception.h"
#include "ioutil.h"
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
    privilege_task_create(foo);
  }

  idle();
}

void idle(void) {
  while (true) {
    printf("CPU0 is in idle state..." EOL);

    while (get_current_task()->timeslice != 0) {}
    get_current_task()->timeslice = DEFAULT_TIMESLICE;
    schedule();
  }
}

void foo(void) {
  while (true) {
    printf("%u..." EOL, get_current_task()->id);

    while (get_current_task()->timeslice != 0) {}
    get_current_task()->timeslice = DEFAULT_TIMESLICE;
    schedule();
  }
}
