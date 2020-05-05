#include "peripherals/irq.h"
#include "peripherals/uart0.h"
#include "peripherals/timer.h"
#include "timer.h"
#include "queue.h"
#include "uart0.h"
#include "exception.h"
#include "schedule.h"

uint64_t arm_core_timer_jiffies = 0, arm_local_timer_jiffies = 0;
uint64_t cntfrq_el0, cntpct_el0;

void irq_enable() {
    asm volatile("msr daifclr, #2");
}

void irq_disable() {
    asm volatile("msr daifset, #2");
}

/*
 * Synchronous Exception
 */

void sync_exc_router(unsigned long esr, unsigned long elr) {
    int ec = (esr >> 26) & 0b111111;
    int iss = esr & 0x1FFFFFF;
    if (ec == 0b010101) {  // system call
        switch (iss) {
            case 1:
                uart_printf("Exception return address 0x%x\n", elr);
                uart_printf("Exception class (EC) 0x%x\n", ec);
                uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
                break;
            case 2:
                arm_local_timer_enable();
                break;
            case 3:
                arm_local_timer_disable();
                break;
            case 4:
                asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq_el0)); // get current counter frequency
                asm volatile ("mrs %0, cntpct_el0" : "=r" (cntpct_el0)); // read current counter
                break;
        }
    }
    else {
        uart_printf("Exception return address 0x%x\n", elr);
        uart_printf("Exception class (EC) 0x%x\n", ec);
        uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
    }
}

/*
 * IRQ Exception
 */

void uart_intr_handler() {
    if (*UART0_MIS & 0x10) {           // UARTTXINTR
        while (!(*UART0_FR & 0x10)) {  // RX FIFO not empty
            char r = (char)(*UART0_DR);
            QUEUE_PUSH(read_buf, r);
        }
        *UART0_ICR = 1 << 4;
    }
    else if (*UART0_MIS & 0x20) {           // UARTRTINTR
        while (!QUEUE_EMPTY(write_buf)) {  // flush buffer to TX
            while (*UART0_FR & 0x20) {      // TX FIFO is full
                asm volatile("nop");
            }
            *UART0_DR = QUEUE_POP(write_buf);
        }
        *UART0_ICR = 2 << 4;
    }
}

void arm_core_timer_intr_handler() {
    register unsigned int expire_period = CORE_TIMER_EXPRIED_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" : : "r"(expire_period));

    current_task->counter--;
    if (current_task->counter > 0 || !PREEMPTABLE(current_task->flag)) {
        return;
    }
    current_task->counter = 0;
    current_task->flag |= 1;
}

void arm_local_timer_intr_handler() {
    *LOCAL_TIMER_IRQ_CLR = 0b11 << 30;  // clear interrupt
    uart_printf("Local timer interrupt, jiffies %d\n", ++arm_local_timer_jiffies);
}

void irq_exc_router() {
    unsigned int irq_basic_pending = *IRQ_BASIC_PENDING;
    unsigned int core0_intr_src = *CORE0_INTR_SRC;

    // GPU IRQ 57: UART Interrupt
    if (irq_basic_pending & (1 << 19)) {
        uart_intr_handler();
    }
    // ARM Core Timer Interrupt
    else if (core0_intr_src & (1 << 1)) {
        arm_core_timer_intr_handler();
    }
    // ARM Local Timer Interrupt
    else if (core0_intr_src & (1 << 11)) {
        arm_local_timer_intr_handler();
    }
}