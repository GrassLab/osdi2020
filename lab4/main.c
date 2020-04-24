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
        schedule();
    }
}

void task2(){
    while(1){
        uart_puts("2...\n");
        sleep();
        schedule();
    }
}

void main()
{
    uart_init();

    core_timer_enable();
    init_Queue(&runQueue);

    privilege_task_create(idle);
    privilege_task_create(task1);
    privilege_task_create(task2);

    go_to(&idle_pcb->context);
}
