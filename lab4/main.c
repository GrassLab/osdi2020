#include "uart.h"
#include "mystd.h"
#include "exc.h"
#include "irq.h"
#include "task.h"
#include "timer.h"

void hello(){
    while(1){
        uart_write("hello\n");
        sleep();
        sched_yield();
    }
}

void task(){
    while(1){
        int taskId = get_taskId();
        uart_hex(taskId);
        uart_write(" ...\n");

        // char in[10] = {0};
        // uart_read(in, 5);
        // uart_write("read ret: \n");
        // uart_write(in);
        // uart_write("\n");

        sleep();
        sched_yield();
        // schedule();
    }
}

void user_task(){
    do_exec(task);
}

void to_hello(){
    exec(hello);
}

void user_task_hello(){
    do_exec(to_hello);
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
    privilege_task_create(user_task_hello);

    go_to(&idle_pcb->context);
}
