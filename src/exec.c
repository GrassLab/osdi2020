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
	printf("ESR: %x, address: %x\n", esr, address);
}

/*
 * Synchronous
 */
// we directly discart the significand reserved bits of esr
void sync_router(unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far){
    // read exception source
    switch(esr>>26) {
        case 0b010101: 
            sync_svc_handler(esr, elr); 
            break;
        default: 
            uart_puts("Unknown sync. "); 
            break;
    }
}

void sync_svc_handler(unsigned long esr, unsigned long elr){
    unsigned int time, time_count, time_freq;
    switch(esr&0xFFFFFF) {
        case 1: //exc
            printf("Exception (ELR): 0x%016X\n", elr);
            printf("Exception (EC) : 0x%02X\n", esr>>26);
            printf("Exception (ISS): 0x%06X\n", esr&0xFFFFFF);
            break;
        case 2: //irq
            local_timer_enable();
            core_timer_enable();
            uart_puts("timer interrupt enabled\n");
            break;
        case 3: //disable irq
            local_timer_disable();
            core_timer_disable();
            uart_puts("timer interrupt disabled\n");
            break;
        case 4: //timestamp
            asm volatile("mrs %0, cntpct_el0": "=r"(time_count)::); // read counts of core timer
            asm volatile("mrs %0, cntfrq_el0": "=r"(time_freq)::);  // read frequency of core timer
            time = time_count / (time_freq / 100000);
            printf("[ %d.%ds ]\n",time/100000, time%100000);
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
    // readirq source
    unsigned int src = *CORE0_IRQ_SRC;
    if (src & (1<<1)){
        core_timer_handler();
        printf("\nCore timer interrupt, jiffies %d", core_cnt++);
        core_cnt ++;
    } else if (src & (1<<11)){
        local_timer_handler();
        printf("\nLocal timer interrupt, jiffies %d", local_cnt++);
        local_cnt ++;
    } else {
        uart_puts("Unknown IRQ\n");
    }
}

// void enable_interrupt_controller()
// {
// 	*ENABLE_IRQS_1 |= IRQ_SYSTEM_TIMER_1;
// }

// void handle_irq()
// {
//     unsigned int pending = *IRQ_PENDING_1;
//     switch (pending) {
//         case (IRQ_SYSTEM_TIMER_1):
//             handle_timer_irq();
//             break;
//         default:
//             printf("Unknown pending irq: %x\r\n", pending);
//     }
// }