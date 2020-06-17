#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#include "timer.h"
#define S_MAX 1000

void foo(){
    while(1) {
        // printf("Task id: %d\n", current -> taskid);
        delay(1000000);
        schedule();
    }
}

void idle(){
    while(1){
        schedule();
        delay(1000000);
    }
}

void el1_main(){
    // set up serial console
    uart_init();

    core_timer_enable();

    int N = 3;
    for(int i = 0; i < N; ++i) { // N should > 2
        privilege_task_create(foo, i);
    }

    idle();
    
}

void main(){
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");
    
    shell();
    return;
}
