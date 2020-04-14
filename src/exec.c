#include "exce.h"

#include "printf.h"
#include "string.h"
#include "timer.h"
#include "uart.h"
// https://developer.arm.com/docs/100933/0100/example-exception-handlers
/*
 * Default 
 */

void show_invalid_entry_message(unsigned long esr, unsigned long address)
{
    uart_puts("ESR: 0x");
    uart_hex(esr);
    uart_puts(", address: 0x");
    uart_hex(address);
    uart_puts("\n");
	// printf("ESR: %x, address: %x\n", esr, address);
}

/*
 * Synchronous
 */
// we directly discart the significand reserved bits of esr
void sync_router(unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far){
    // read exception source
    switch(esr>>26) {
        case 0b010101: sync_svc_handler(esr, elr); break;
        default: uart_puts("Unknown sync. "); break;
    }
}

void sync_svc_handler(unsigned long esr, unsigned long elr){
    unsigned int time, time_count, time_freq;
    char buff[10];
    switch(esr&0xFFFFFF) {
        case 1: //exc
            uart_puts("Exception (ELR): 0x");
            uart_hex(elr);
            uart_puts("\nException (EC) : 0x");
            uart_hex(esr>>26);
            uart_puts("\nException (ISS): 0x");
            uart_hex(esr&0xFFFFFF);
            uart_puts("\n");
            // printf("Exception (ELR): 0x%016X\n", elr);
            // printf("Exception (EC) : 0x%02X\n", esr>>26);
            // printf("Exception (ISS): 0x%06X\n", esr&0xFFFFFF);
            break;
        case 2: //irq
            enable_irq();
            core_timer_enable();
            // local_timer_init();
            uart_puts("timer interrupt enabled\n");
            break;
        case 3: //timestamp
            asm volatile("mrs %0, cntpct_el0": "=r"(time_count)::); // read counts of core timer
            asm volatile("mrs %0, cntfrq_el0": "=r"(time_freq)::);  // read frequency of core timer
            time = time_count / (time_freq / 100000);
            // printf("[ %d.%ds ]\n",time/100000, time%100000);	
            unitoa((time/100000U), buff, 5);
            uart_puts(buff); // natural part
            uart_send('.');
            unitoa(time%100000U, buff, 5);
            uart_puts(buff); // decimal part
            uart_puts("\n");
            break;
        default: uart_puts("Unknown SVC\n"); break;
    }
}

/*
 * IRQ
 */
// https://github.com/raspberrypi/documentation/blob/master/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
unsigned int core_cnt = 0;
unsigned int local_cnt = 0;
void irq_router(){
    char buff[10];
    // readirq source
    unsigned int src = *CORE0_IRQ_SRC;
    if (src & (1<<1)){
        core_timer_handler();
        uart_puts("\nCore timer interrupt, jiffies ");
        unitoa(core_cnt, buff, 1);
        uart_puts(buff);
        core_cnt ++;
        // printf("Core timer interrupt, jiffies %d\n", core_cnt++);
    } else {
        uart_puts("Unknown IRQ\n");
    }
}