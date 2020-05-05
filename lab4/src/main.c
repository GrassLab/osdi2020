#include "uart.h"
#include "rpifunc.h"
#include "shell.h"
#include "printf.h"
#include "sched.h"
#include "timer.h"

#define N 1

extern void disable_irq();
extern int get_taskid();
extern int exec();

void foo(){
  int tmp = 5;
  printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
}

void idle(){
  // uart_puts("enter idel\n");
  while(1){
    idle_schedule();
    delay(1000000);
  }
}

void test() {
  int cnt = 1;
  // if (fork() == 0) {
  //   fork();
  //   delay(100000);
  //   fork();
    // while(cnt < 100) {
    //   printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
    //   delay(100000);
    //   ++cnt;
    //   // schedule();
    // }
  //   exit(0);
  //   printf("Should not be printed\n");
  // } else {
    printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
    exec(foo);
  // }
}

void user_test(){
  // test();
  uart_puts("kernel calls do_exec in order to switch to user level\n");
  do_exec(test);
}

void main()
{
    PL011_uart_init(4000000);
    show_boot_msg();
    init_printf(0, putc);
    init_task();
    for(int i = 0; i < N; ++i)  
        //  privilege_task_create(foo);
        privilege_task_create(user_test);
    // disable_irq();
    core_timer_enable();
    idle();

}
