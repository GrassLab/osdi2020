#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "timer.h"
#include "sys.h"
#include "irq.h"

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
        enable_interrupt_controller();
        unsigned int cntfrq;
        unsigned int val;
        cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
        val = read_cntp_tval();     // read 
        core_timer_enable();
        enable_irq();
    }
    if (imm_value == SYS_EXC) {
        uart_send_hex(address);
        uart_send_hex(esr>>26);
        uart_send_hex(esr & 0xfff);
    }
    if (imm_value == SYS_UART) {

        // TODO enable uart
        enable_uart_interrupt();
        enable_irq();
    }
    return;
}