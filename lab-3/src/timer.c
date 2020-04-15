#include "timer.h"
#include "utility.h"
#include "uart.h"

int local_timer_counter = 0;
int core_timer_counter = 0;

void local_timer_init()
{
    asm volatile("svc #0x102");
}

void __local_timer_init()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    setRegister(LOCAL_TIMER_CONTROL_REG, flag | reload);
}

void local_timer_handler()
{
    uart_puts("[timer] Local timer interrupt, repeat ");
    uart_print_int(local_timer_counter++);
    uart_puts(" times\n");
    setRegister(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
}

void sys_timer_init()
{
    asm volatile("svc #0x103");
}

void __sys_timer_init()
{
    unsigned int t = getRegister(SYSTEM_TIMER_CLO);
    setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
    setRegister(IRQ_ENABLE0, 1 << 1);
}

void sys_timer_handler()
{
    unsigned int t = getRegister(SYSTEM_TIMER_CLO);
    setRegister(SYSTEM_TIMER_COMPARE1, t + 2500000);
    setRegister(SYSTEM_TIMER_CS, 0xf);
}

void core_timer_init()
{
    asm volatile("svc #0x104");
}

void __core_timer_init()
{
    asm volatile("mov x0, 1");
    asm volatile("msr cntp_ctl_el0, x0");
    asm volatile("mov x0, 0xfffffff");
    asm volatile("msr cntp_tval_el0, x0");
    asm volatile("mov x0, 2");
    asm volatile("ldr x1, =0x40000040");
    asm volatile("str x0, [x1]");
}

void core_timer_handler()
{
    uart_puts("[timer] Core timer interrupt, ");
    uart_print_int(core_timer_counter++);
    uart_puts(" times\n");
    asm volatile("mov x0, 0xfffffff");
    asm volatile("msr cntp_tval_el0, x0");
}