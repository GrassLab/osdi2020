#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "timer.h"
#include "sys.h"
#include "../include/peripherals/irq.h"

unsigned int read_cntfrq(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

void write_cntp_tval(unsigned int val)
{
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}

unsigned int read_cntp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}

void enable_uart_interrupt()
{
	put32(ENABLE_IRQS_1, AUX_IRQ);
	return;
}


void handle_sync(unsigned long esr, unsigned long address)
{
    uart_send_hex(address);
    uart_send_hex(esr>>26);
    uart_send_hex(esr & 0xfff);
    return;
}

void handle_el0_sync(unsigned long esr, unsigned long address)
{
    unsigned int *instruct_value = address - 4;
    unsigned int imm_value = (*instruct_value & ~0xfff00000) >> 5;
    if (imm_value == SYS_TIME_IRQ) {
        sys_timer_init();
        enable_timer_controller();  // function in irq.c
        unsigned int cntfrq;
        unsigned int val;
        cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
        val = read_cntp_tval();     // read 
        core_timer_enable();
        enable_irq();
    }
    if (imm_value == SYS_EXC) {
        uart_send_string("return address ");
        uart_send_hex(address);
	    uart_send_string("(EC) ");
        uart_send_hex(esr>>26);
        uart_send_string("(ISS) ");
        uart_send_hex(esr & 0xfff);
    }
    if (imm_value == SYS_UART) {
        enable_uart_interrupt();
        enable_irq();
    }
    return;
}