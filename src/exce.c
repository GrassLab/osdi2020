#include "exce.h"

#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "uart.h"
// https://developer.arm.com/docs/100933/0100/example-exception-handlers
/*
 * Default 
 */

void show_invalid_entry_message(unsigned long esr, unsigned long address)
{
	printf("[Invalid Exception]\tESR: %x, address: %x\n", esr, address);
}

void sync_svc_handler(unsigned long esr, unsigned long elr, struct trapframe *tf)
{
    // unsigned int time, time_count, time_freq;
    // switch(esr&0xFFFFFF) {
    switch(tf->Xn[8]) {
        case SYS_UART_READ:
            tf->Xn[0] = uart_getc(); 
            break;
        case SYS_UART_WRITE:
            uart_puts((char *)tf->Xn[0]); 
            break;
        case SYS_GET_TASKID:
            tf->Xn[0] = current->task_id; 
            break;
        case SYS_EXEC:
            do_exec((void*)tf->Xn[0]); 
            break;
        case SYS_FORK:
            do_fork(tf);
            break;
        case SYS_EXIT:
            do_exit(tf->Xn[0]); 
            break;
        case SYS_SCHED_YIELD:
            schedule(); 
            break;
        default:
            printf("Unknown system call ID: %d\n", tf->Xn[8]); 
            break;
    }
}

/*
 * Synchronous
 */
// we directly discart the significand reserved bits of esr
void sync_router(unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far, struct trapframe *tf){
    // read exception source
    switch(esr>>26) {
        case 0b010101: 
            sync_svc_handler(esr, elr, tf); 
            break;
        default: 
            uart_puts("Unknown sync. ");
            while(1);
            break;
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
        printf("[Irq router] core timer interrupt trigerred!\n");
        core_timer_handler();
        timer_tick();
        // printf("\nCore timer interrupt, jiffies %d", core_cnt++);
        // core_cnt ++;
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