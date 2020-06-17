#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#define S_MAX 1000

void first_func() {
    while (1) {
        delay(1000000);
        schedule();
    }
}

void second_func() {
    while (1) {
        delay(1000000);
        schedule();
    }
}

void third_func() {
    while (1) {
        delay(1000000);
        schedule();
    }
}

void el1_main(){
    // set up serial console
    uart_init();

    privilege_task_create(first_func, 0);
    privilege_task_create(second_func, 0);
    privilege_task_create(third_func, 0);


    while(1){
        schedule();
    }
}

void main(){
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");
    
    shell();
    return;
}
