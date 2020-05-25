#include "asm.h"
#include "uart.h"
#include "timer.h"
#include "shed.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;
int core_timer_counter = 0;
int local_timer_counter = 0;

void timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
    uart_puts("timer init\r\n");
}

void handle_timer_irq( void ) 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	uart_puts("Timer interrupt received\n\r");
}

void core_timer_init()
{
    // asm volatile("stp x29, x30, [sp, #-16]!");
    // asm volatile("mov x29, sp");

    // asm_delay();
    asm volatile("mov x0, 1");
    asm volatile("msr cntp_ctl_el0, x0");
    asm volatile("mrs x0, cntfrq_el0");
    asm volatile("msr cntp_tval_el0, x0");
    asm volatile("mov x0, 2");
    asm volatile("ldr x1, =0x40000040");
    asm volatile("str x0, [x1]");

    // asm volatile("ldp x29, x30, [sp], #16");
    // asm volatile("ret");
}

void core_timer_handler()
{
    uart_puts("Core timer interrupt, ");
    uart_print_int(core_timer_counter++);
    uart_puts(" times\n");
    // timer_tick();
    asm volatile("mrs x0, cntfrq_el0");
    asm volatile("msr cntp_tval_el0, x0");
}

void local_timer_init()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    put32(LOCAL_TIMER_CONTROL_REG, flag | reload);
}

void local_timer_handler()
{
    // uart_puts("Local timer interrupt, repeat ");
    // uart_print_int(local_timer_counter++);
    // uart_puts(" times\n");
    put32(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
}
