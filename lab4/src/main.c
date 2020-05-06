#include "uart.h"
#include "rpifunc.h"
#include "shell.h"
#include "printf.h"
#include "sched.h"
#include "timer.h"
#include "sys.h"
#include "mystd.h"

#define N 10

extern void disable_irq();

char out[128] = {0};
char buf[32] = {0};

void foo()
{
  int tmp = 5;
  strAppend(out, "Task ");
  itoa((unsigned long long)get_taskid(), buf, 10);
  strAppend(out, buf);
  strAppend(out, " after exec, tmp address ");
  itoa((unsigned long long)&tmp, buf, 16);
  strAppend(out, buf);
  strAppend(out, ", tmp value ");
  itoa((unsigned long long)tmp, buf, 10);
  strAppend(out, buf);
  strAppend(out, "\n");
  uart_write(out);
  out[0] = '\0';
  // printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
  exit(0);
}

void test()
{
  int cnt = 1;
  if (fork() == 0){
    fork();
    delay(100000);
    fork();
    while (cnt < 100){
       strAppend(out, "Task id: ");
       itoa((unsigned long long)get_taskid(), buf, 10);
       strAppend(out, buf); 
       strAppend(out, ", cnt: ");
       itoa((unsigned long long)cnt, buf, 10);
       strAppend(out, buf);
       strAppend(out, "\n");
       uart_write(out);
       out[0] = '\0';
      // printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
      delay(100000);
      ++cnt;
    }
    exit(0);
    uart_write("Should not be printed\n");
  }
  else{
    strAppend(out, "Task ");
    itoa((unsigned long long)get_taskid(), buf, 10);
    strAppend(out, buf);
    strAppend(out, " before exec, tmp address ");
    itoa((unsigned long long)&cnt, buf, 16);
    strAppend(out, buf);
    strAppend(out, ", cnt value ");
    itoa((unsigned long long)cnt, buf, 10);
    strAppend(out, buf);
    strAppend(out, "\n");
    uart_write(out);
    out[0] = '\0';
    // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
    exec(foo);
  }
}

void user_test()
{
  // uart_puts("enter user_test\n");
  do_exec(test);
}

void idle()
{
  // uart_puts("enter idel\n");
  // while(1){
  //   idle_schedule();
  //   delay(1000000);
  // }
  while (1){
    if (num_runnable_tasks() == 1){
      break;
    }
    idle_schedule();
    delay(1000000);
  }
  uart_write("Test finished\n");
  while (1);
}

void main()
{
  PL011_uart_init(4000000);
  show_boot_msg();
  init_printf(0, putc);
  init_task();
  // for (int i = 0; i < N; ++i)
  privilege_task_create(user_test);
  // disable_irq();
  core_timer_enable();
  idle();
}
