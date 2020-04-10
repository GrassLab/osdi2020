#include "timer.h"
#include "utility.h"
#include "uart.h"

int local_timer_counter = 0;

void local_timer_init(){
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    setRegister(LOCAL_TIMER_CONTROL_REG, flag | reload);
}

void local_timer_handler() {
    uart_puts("Local timer interrupt, repeat ");
    uart_print_int(local_timer_counter);
    uart_puts(" times\n");
    local_timer_counter ++;
    setRegister(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
    asm volatile("eret");
}

void sys_timer_init(){
    unsigned int t = getRegister(SYSTEM_TIMER_CLO);
    setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
    setRegister(IRQ_ENABLE0, 1 << 1);
}

void sys_timer_handler(){
    unsigned int t = getRegister(SYSTEM_TIMER_CLO);
    setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
    setRegister(SYSTEM_TIMER_CS, 0xf);
}
