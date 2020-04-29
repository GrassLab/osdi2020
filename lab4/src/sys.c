#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "timer.h"
#include "sys.h"
#include "fork.h"
#include "irq.h"
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

void handle_sync(unsigned long esr, unsigned long address)
{
    uart_send_hex(address);
    uart_send_hex(esr>>26);
    uart_send_hex(esr & 0xfff);
    return;
}

void handle_el0_sync(unsigned long esr, unsigned long address)
{
    unsigned int val;
    asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_CLONE) {
        uart_send("return from child\r\n");
        return copy_process(0, 0, 0, 0);
    }
    return 0;
}

void handle_el1_sync(unsigned long esr, unsigned long address)
{
    unsigned int val;
	asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_TIME_IRQ) {
        sys_timer_init();
        enable_timer_controller();  // function in irq.c
        unsigned int cntfrq;
        unsigned int val;
        cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
        val = read_cntp_tval();
        core_timer_enable();
        enable_irq();
    }
    return;
}