#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"
#include "timer.h"
#include "irq.h"
#include "config.h"
#include "thread.h"

extern task_manager_t TaskManager;
extern task_t* current;

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

        task_t* new_task = &TaskManager.task_pool[2];
        // printf("print2 content\n");
        // printf("%d\n",new_task->task_id);
        // printf("%x\n",new_task->cpu_context.x19);
        // printf("%x\n",new_task->cpu_context.x20);
        // printf("%x\n",new_task->cpu_context.pc);
        // printf("%x\n",new_task->cpu_context.sp);


        context_switch(new_task);
    }
}

void print2(){
    while (1){
        printf("---\n");
        printf("222\n");
        delay(100000000);

        task_t* new_task = &TaskManager.task_pool[1];
        // printf("print2 content\n");
        // printf("%d\n",new_task->task_id);
        // printf("%x\n",new_task->cpu_context.x19);
        // printf("%x\n",new_task->cpu_context.x20);
        // printf("%x\n",new_task->cpu_context.pc);
        // printf("%x\n",new_task->cpu_context.sp);


        context_switch(new_task);
    }
}

void main()
{
    init_uart();
    init_lfb();
    init_printf(0, putc);
    init_task_manager();

    // irq_vector_init();
    // timer_init();
    // enable_interrupt_controller();

    // asm volatile ("svc #1");
	// enable_irq();



    task_t* new_task1 = privilege_task_create((unsigned long)&print, 1);
    printf("%d\n",new_task1->task_id);
    printf("%x\n",new_task1->cpu_context.x19);
    printf("%x\n",new_task1->cpu_context.x20);
    printf("%x\n",new_task1->cpu_context.pc);
    printf("%x\n",new_task1->cpu_context.sp);
    task_t* new_task2 = privilege_task_create((unsigned long)&print2, 2);

    // shell();
    schedule();



    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }

}
