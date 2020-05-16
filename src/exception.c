#include "peripherals/irq.h"
#include "peripherals/uart0.h"
#include "peripherals/timer.h"
#include "timer.h"
#include "queue.h"
#include "uart0.h"
#include "exception.h"
#include "schedule.h"
#include "sys.h"

char intr_stack[INTR_STK_SIZE];
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

void sys_get_task_id(struct trapframe* trapframe) {
    uint64_t task_id = get_current_task()->id;
    trapframe->x[0] = task_id;
}

void sys_uart_read(struct trapframe* trapframe) {
    char* buf = (char*) trapframe->x[0];
    uint32_t size = trapframe->x[1];

    irq_enable();
    for (int i = 0; i < size; i++) {
        buf[i] = uart0_read();
    }
    buf[size] = '\0';
    irq_disable();
    trapframe->x[0] = size;
}

void sys_uart_write(struct trapframe* trapframe) {
    const char* buf = (char*) trapframe->x[0];
    uint32_t size = trapframe->x[1];

    irq_enable();
    for (int i = 0; i < size; i++) {
        uart0_write(buf[i]);
    }
    irq_disable();
    trapframe->x[0] = size;
}

void sys_exec(struct trapframe* trapframe) {
    void (*func)() = (void(*)()) trapframe->x[0];
    do_exec(func);
    trapframe->x[0] = 0;
}

void sys_call_router(uint64_t sys_call_num, struct trapframe* trapframe) {
    switch (sys_call_num) {
        case SYS_GET_TASK_ID:
            sys_get_task_id(trapframe);
            break;

        case SYS_UART_READ:
            sys_uart_read(trapframe);
            break;

        case SYS_UART_WRITE:
            sys_uart_write(trapframe);
            break;

        case SYS_EXEC:
            sys_exec(trapframe);
            break;
    }
}

void sync_exc_router(unsigned long esr, unsigned long elr, struct trapframe* trapframe) {
    int ec = (esr >> 26) & 0b111111;
    int iss = esr & 0x1FFFFFF;
    if (ec == 0b010101) {  // system call
        uint64_t syscall_num = trapframe->x[8];
        sys_call_router(syscall_num, trapframe);
        // switch (iss) {
        //     case 1:
        //         uart_printf("Exception return address 0x%x\n", elr);
        //         uart_printf("Exception class (EC) 0x%x\n", ec);
        //         uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
        //         break;
        //     case 2:
        //         arm_core_timer_enable();
        //         arm_local_timer_enable();
        //         break;
        //     case 3:
        //         arm_core_timer_disable();
        //         arm_local_timer_disable();
        //         break;
        //     case 4:
        //         asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq_el0)); // get current counter frequency
        //         asm volatile ("mrs %0, cntpct_el0" : "=r" (cntpct_el0)); // read current counter
        //         break;
        // }
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
            queue_push(&read_buf, r);
        }
        *UART0_ICR = 1 << 4;
    }
    else if (*UART0_MIS & 0x20) {           // UARTRTINTR
        while (!queue_empty(&write_buf)) {  // flush buffer to TX
            while (*UART0_FR & 0x20) {      // TX FIFO is full
                asm volatile("nop");
            }
            *UART0_DR = queue_pop(&write_buf);
        }
        *UART0_ICR = 2 << 4;
    }
}

void arm_core_timer_intr_handler() {
    register unsigned int expire_period = CORE_TIMER_EXPRIED_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" : : "r"(expire_period));

    // check current task running time
    struct task_t *current = get_current_task();
    if (--current->counter <= 0) {
        current->counter = 0;
        current->need_resched = 1;
    }
    irq_enable();
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

void irq_stk_switcher() {
    // Switch to interrupt stack if entry_sp in kernel stack
    register char* entry_sp;
    asm volatile("mov %0, sp": "=r"(entry_sp));
    if (!(entry_sp <= &intr_stack[4095] && entry_sp >= &intr_stack[0])) {
        asm volatile("mov sp, %0" : : "r"(&intr_stack[4095]));
    }

    irq_exc_router();

    // Restore to kernel stack if entry_sp in kernel stack
    if (!(entry_sp <= &intr_stack[4095] && entry_sp >= &intr_stack[0])) {
        asm volatile("mov sp, %0" : : "r"(entry_sp));
    }
}
