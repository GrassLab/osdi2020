#include "exce.h"
#include "mailbox.h"
#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "uart.h"

void delay(int cnt)
{
    while(cnt--);
}

void wait(long task_id){
    #ifdef __DEBUG
    printf("[wait] Task_id: %d\n", current->task_id);
    #endif//__DEBUG
    while (1)
    {
        if(task_pool[task_id]->state == TASK_ZOMBIE){
            task_pool[task_id] = 0;
            break;
        }
        sys_sched_yield();
    }
    
}

void foo(){// user code
    int tmp = 5;
    printf("Task %d after  exec, tmp address 0x%x, tmp value %d\n", sys_get_taskid(), &tmp, tmp);
    sys_exit(0);
}

void test() {//user code
    int cnt = 1;
    int pid;
    #ifdef __DEBUG
    printf("[test] Task_id: %d, cnt: %d\n", current->task_id, cnt);
    #endif//__DEBUG
    if ((pid = sys_fork()) == 0) {
        sys_fork();
        delay(100000);
        sys_fork();
        #ifdef __DEBUG
        printf("[tast] Task_id: %d, cnt: %d\n", current->task_id, cnt);
        #endif//__DEBUG
        while(cnt < 10){
            printf("Task id: %d, cnt: %d\n", sys_get_taskid(), cnt);
            delay(100000);
            ++cnt;
        }
        sys_exit(0);
    printf("Should not be printed\n");
    } else {
        printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", sys_get_taskid(), &cnt, cnt);
        wait(pid);
        sys_exec(foo);
  }
}

void user_test(){//kernel code
    do_exec(test);
}

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();

    // create test process
    privilege_task_create(user_test);
    // for(int i = 0; i < 5; ++i) { // N should > 2
    //     privilege_task_create(foo);
    // }

    core_timer_enable();

    idle_task();
    return -1;
}
