#include "uart.h"
#include "rpifunc.h"
#include "shell.h"
#include "printf.h"
#include "sched.h"

#define N 10

void foo(){
  while(1) {
    printf("Task id: %d\n", current -> taskid);
    delay(1000000);
    schedule();
    // if (current -> taskid != 10)
    //     context_switch(&task_pool[current -> taskid + 1]);
    // else
    //     context_switch(&task_pool[1]);
  }
}

void idle(){
  // uart_puts("enter idel\n");
  while(1){
    schedule();
    delay(1000000);
  }
}

void main()
{
    PL011_uart_init(4000000);
    show_boot_msg();
    // run_shell();
    init_printf(0, putc);
    init_task();
    for(int i = 0; i < N; ++i)  // N should > 2
         privilege_task_create(foo);

    // context_switch(&task_pool[1]);

    idle();

    // privilege_task_create(foo);
    // privilege_task_create(foo);
    // printf("Task id: %d\n", current -> taskid);
    // uart_hex(current); uart_puts("\n");
    // uart_hex(&task_pool[0]); uart_puts("\n");
    // uart_hex(&task_pool[1]); uart_puts("\n");
    // while(1){
    //     context_switch(&task_pool[0]);
    //     uart_hex(current); uart_puts("...1\n");
    //     printf("Task id: %d\n", current -> taskid);
    //     context_switch(&init);
    //     context_switch(&task_pool[1]);
    //     uart_hex(current); uart_puts("...2\n");
    //     printf("Task id: %d\n", current -> taskid);
    // }
}
