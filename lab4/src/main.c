#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"
#include "timer.h"
#include "irq.h"
#include "config.h"
#include "thread.h"

task_manager_t TaskManager;
task_t* current;

void init_uart(){
    // uart_init();
    get_board_revision();
    get_vc_memory();
    get_UART_clock();
    set_UART_clock();
    get_UART_clock();
}

void init_lfb(){
    lfb_init();
    lfb_showpicture();
}

void print(){
    while (1){
        printf("---\n");
        printf("111\n");
        delay(100000000);

        // printf("print content\n");
        // printf("%d\n",current->task_id);
        // printf("%x\n",current->cpu_context.x19);
        // printf("%x\n",current->cpu_context.x20);
        // printf("%x\n",current->cpu_context.pc);
        // printf("%x\n",current->cpu_context.sp);

        task_t* self_task = &TaskManager.task_pool[1];
        if(self_task->counter <= 0){
            schedule();
        }
    }
}

void print2(){
    while (1){
        printf("---\n");
        printf("222\n");
        delay(100000000);

        task_t* self_task = &TaskManager.task_pool[2];

        if(self_task->counter <= 0){
            schedule();
        }
    }
}

void print3(){
    while (1){
        printf("---\n");
        printf("333\n");
        delay(100000000);

        task_t* self_task = &TaskManager.task_pool[3];

        if(self_task->counter <= 0){
            schedule();
        }
    }
}

void main()
{
    init_uart();
    init_lfb();
    init_printf(0, putc);
    init_task_manager();

    enable_interrupt_controller();
    core_timer_enable();
	enable_irq();



    task_t* new_task1 = privilege_task_create((unsigned long)&print);
    task_t* new_task2 = privilege_task_create((unsigned long)&print2);
    task_t* new_task3 = privilege_task_create((unsigned long)&print3);

    // shell();
    schedule();



    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }

}
