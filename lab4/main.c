#include "uart.h"
#include "mystd.h"
#include "exc.h"
#include "irq.h"
#include "task.h"
#include "timer.h"

void task(){
    while(1){
        int taskId = get_taskId();
        uart_hex(taskId);
        uart_write(" ...\n");
        sleep();
        sched_yield();
        // schedule();
    }
}

void user_task(){
    do_exec(task);
}

void main()
{
    uart_init();

    core_timer_enable();
    init_Queue(&runQueue);

    privilege_task_create(idle);
    // privilege_task_create(task1);
    // privilege_task_create(task2);
    // privilege_task_create(task3);
    privilege_task_create(user_task);
    privilege_task_create(user_task);

    go_to(&idle_pcb->context);
}
