#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#define S_MAX 1000

void wait_msec(unsigned int n) {
    register unsigned long f, t, r;
    // get the current counter frequency
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
    // read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));
    // calculate expire value for counter
    t += ((f / 1000) * n) / 1000;
    do {
        asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < t);
}

void first_func() {
    while (1) {
        uart_puts("function 1...\n");
        wait_msec(600000);
        context_switch(task[0]);
    }
}

void second_func() {
    while (1) {
        uart_puts("function 2...\n");
        wait_msec(600000);
        context_switch(task[0]);
    }
}

void el1_main(){
    // set up serial console
    uart_init();

    privilege_task_create(first_func, 0);
    privilege_task_create(second_func, 0);

    int i=0;
    int x=0;
    while(1){
        x = i++%2+1;
        context_switch(task[x]);
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
