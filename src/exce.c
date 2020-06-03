#include "exce.h"

#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "uart.h"

/*
 * Default Handler
 */
void show_invalid_entry_message(unsigned long esr, unsigned long address)
{
	printf("[Invalid Exception]\tESR: %x, address: %x\n", esr, address);
}

static void sync_svc64_handler(unsigned long esr, unsigned long elr, struct trapframe *tf)
{
    switch(tf->Xn[8]) {
        case SYS_UART_READ: tf->Xn[0] = uart_getc(); break;
        case SYS_UART_WRITE: uart_puts((char *)tf->Xn[0]); break;
        case SYS_GET_TASKID: tf->Xn[0] = do_get_taskid(); break;
        case SYS_EXEC: do_exec((void*)tf->Xn[0]); break;
        case SYS_FORK: do_fork(tf); break;
        case SYS_EXIT: do_exit(tf->Xn[0]); break;
        case SYS_SCHED_YIELD: schedule();  break;
        default:
            printf("Unknown system call ID: %d\n", tf->Xn[8]);
            break;
    }
}

/*
 * Synchronous
 */
void sync_router(unsigned long esr, unsigned long elr, struct trapframe *tf){
    // read exception source
    switch(esr>>26) {
        case 0b010101: sync_svc64_handler(esr, elr, tf); break;
        default:
            printf("Unknown sync. EC: 0x%X\t ISS: 0x%X", esr>>26, esr&0x7fffff);
            while(1);
            break;
    }
}

/*
 * IRQ
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
 */
void irq_router(){
    // static unsigned int core_cnt = 0;
    static unsigned int local_cnt = 0;
    unsigned int src = *CORE0_IRQ_SRC;
    if (src & IRQ_SRC_CNTPNS){
        printf("[Irq router] core timer interrupt trigerred!\n");
        core_timer_handler();
        timer_tick();
        // printf("\nCore timer interrupt, jiffies %d", core_cnt++);
        // core_cnt ++;
    } else if (src & IRQ_SRC_LOCAL_TIMER){
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