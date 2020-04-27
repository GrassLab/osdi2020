#include "uart.h"
#include "mystd.h"
#include "exc.h"
#include "irq.h"
#include "task.h"
#include "timer.h"

void task1(){
    while(1){
        uart_puts("1...\n");
        sleep();
        sched_yield();
        // schedule();
    }
}

void task2(){
    while(1){
        uart_puts("2...\n");
        sleep();
        sched_yield();
        // schedule();
    }
}

void task3(){
    while(1){
        uart_puts("3...\n");
        sleep();
        schedule();
    }
}

void task(){
    uart_puts("user task\n");
    while(1);
}

void user_task1(){
    do_exec(task1);
}

void user_task2(){
    do_exec(task2);
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
    privilege_task_create(user_task1);
    privilege_task_create(user_task2);

    go_to(&idle_pcb->context);
}
