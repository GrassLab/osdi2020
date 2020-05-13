#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "framebuffer.h"
#include "timer.h"
#include "irq.h"
#include "config.h"
#include "thread.h"
#include "syscall.h"

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

        if(current->counter <= 0){
            schedule();
        }
    }
}

void print2(){
    while (1){
        printf("---\n");
        printf("222\n");
        delay(100000000);

        if(current->counter <= 0){
            schedule();
        }
    }
}

void print3(){
    while (1){
        printf("---\n");
        printf("333\n");
        delay(100000000);

        if(current->counter <= 0){
            schedule();
        }
    }
}

void func(){
    int cnt = 1;
    while(1){
        printf("---\n");
        printf("user loop\n");
        printf("kernel stack location: %x\n",current->cpu_context.sp);
        printf("user stack location: %x\n",current->user_context.sp_el0);
        printf("taskid: %d\n", get_taskid());
        printf("user loop cnt: %d, address of cnt: %d\n", cnt, &cnt);

        cnt++;
        // printf("---\n");

        delay(10000000);
    }
}

void func2(){
    int cnt = 1;
    while(1){
        printf("---\n");
        printf("user loop2\n");
        printf("kernel stack location: %x\n",current->cpu_context.sp);
        printf("user stack location: %x\n",current->user_context.sp_el0);
        printf("taskid: %d\n", get_taskid());
        printf("user loop cnt: %d, address of cnt: %d\n", cnt, &cnt);
        cnt++;
        // printf("---\n");

        delay(10000000);
    }
}
 
void user_task2(){
    do_exec(func2);
}

void user_task(){
    do_exec(func);
}

void main()
{
    init_uart();
    init_lfb();
    init_printf(0, putc);
    printf("fsaf\n");

    init_task_manager();


    // task_t* new_task1 = privilege_task_create((unsigned long)&print);
    // task_t* new_task2 = privilege_task_create((unsigned long)&print2);
    // task_t* new_task3 = privilege_task_create((unsigned long)&print3);
    task_t* new_task4 = privilege_task_create((unsigned long)&user_task);
    task_t* new_task5 = privilege_task_create((unsigned long)&user_task2);

    // shell();
    enable_interrupt_controller();
    core_timer_enable();
	enable_irq();


    schedule();




    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }

}
