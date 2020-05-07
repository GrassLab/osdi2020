#include "peripherals/irq.h"
#include "peripherals/uart0.h"
#include "peripherals/timer.h"
#include "timer.h"
#include "queue.h"
#include "uart0.h"
#include "exception.h"
#include "schedule.h"
#include "sys.h"
#include "mm.h"
#include "signal.h"

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
void _k_sys_get_cntfrq(struct trapframe* trapframe) {
    uint64_t cntfrq_el0;
    asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq_el0)); // get current counter frequency
    trapframe->x[0] = cntfrq_el0;
}

void _k_sys_get_cntpct(struct trapframe* trapframe) {
    uint64_t cntpct_el0;
    asm volatile ("mrs %0, cntpct_el0" : "=r" (cntpct_el0)); // read current counter
    trapframe->x[0] = cntpct_el0;
}

void _k_sys_uart_read(struct trapframe* trapframe) {
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

void _k_sys_uart_write(struct trapframe* trapframe) {
    const char* buf = (char*) trapframe->x[0];
    uint32_t size = trapframe->x[1];

    irq_enable();
    for (int i = 0; i < size; i++) {
        uart0_write(buf[i]);
    }
    irq_disable();
    trapframe->x[0] = size;
}

void _k_sys_exec(struct trapframe* trapframe) {
    void (*func)() = (void(*)()) trapframe->x[0];
    do_exec(func);
    trapframe->x[0] = 0;
}

void _k_sys_exit(struct trapframe* trapframe) {
    do_exit(trapframe->x[0]);
}

void _k_sys_fork(struct trapframe* trapframe) {
    int child_id = privilege_task_create(return_from_fork);
    if (child_id < 0) { // create failed
        trapframe->x[0] = child_id;
        return;
    }

    struct task_struct* child_task = &task_pool[child_id];
    struct task_struct* parent_task = current_task;
    child_task->ustack = get_avaliable_ustack();

    char* parent_kstack = parent_task->kstack;
    char* parent_ustack = parent_task->ustack;
    char* child_kstack = child_task->kstack;
    char* child_ustack = child_task->ustack;
    if (!child_ustack) {
        child_task->state = EXIT; // will ignore this task when scheduling
        trapframe->x[0] = child_id;
        return;
    }

    for (int i = 0; i < KSTK_SIZE; i++) {
        *(child_kstack - i) = *(parent_kstack - i);
    }
    for (int i = 0; i < USTK_SIZE; i++) {
        *(child_ustack - i) = *(parent_ustack - i);
    }

    // place child's kernel stack to right place
    child_task->cpu_context.sp = (uint64_t)child_kstack - (parent_kstack - (char*)trapframe);

    // place child's user stack to right place
    struct trapframe* child_trapframe = (struct trapframe*) child_task->cpu_context.sp;
    child_trapframe->sp_el0 = (uint64_t)child_ustack - (parent_ustack - (char*)trapframe->sp_el0);

    child_trapframe->x[0] = 0;
    trapframe->x[0] = child_task->id;
}

void _k_sys_kill(struct trapframe* trapframe) {
    uint32_t pid = trapframe->x[0];
    int sig = trapframe->x[1];

    switch (sig) {
        case SIG_KILL:
            SET(task_pool[pid].flag, KILL_BIT);
            break;
    }
}

void sys_call_router(uint64_t sys_call_num, struct trapframe* trapframe) {
    switch (sys_call_num) {
        case SYS_GET_CNTFRQ:
            _k_sys_get_cntfrq(trapframe);
            break;

        case SYS_GET_CNTPCT:
            _k_sys_get_cntpct(trapframe);
            break;

        case SYS_UART_READ:
            _k_sys_uart_read(trapframe);
            break;

        case SYS_UART_WRITE:
            _k_sys_uart_write(trapframe);
            break;

        case SYS_EXEC:
            _k_sys_exec(trapframe);
            break;

        case SYS_FORK:
            _k_sys_fork(trapframe);
            break;

        case SYS_EXIT:
            _k_sys_exit(trapframe);
            break;

        case SYS_KILL:
            _k_sys_kill(trapframe);
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
        //         arm_local_timer_enable();
        //         break;
        //     case 3:
        //         arm_local_timer_disable();
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

    // check flag has SIGKILL
    if (HAS(current_task->flag, KILL_BIT)) {
        do_exit(0);
        return;
    }

    current_task->counter--;
    if (current_task->counter > 0) {
        return;
    }
    uart_printf("reschedule from %d\n", current_task->id);
    current_task->counter = 0;
    SET(current_task->flag, RESCHEDULE_BIT);
    irq_enable();
    schedule();
    irq_disable();
    CLR(current_task->flag, RESCHEDULE_BIT);
    current_task->counter = current_task->priority;
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