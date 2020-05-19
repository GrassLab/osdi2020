#include "irq.h"
#include "time.h"
#include "syscall.h"
#include "utility.h"
#include "task.h"

static void el0_svc_handler(unsigned long sp)
{
    Task *task = get_current();
    Trapframe *trapframe = task->trapframe;
    unsigned long x8 = trapframe->regs[8];
    unsigned long x0 = trapframe->regs[0];
    unsigned long syscallReturnValue = 0;

    if (x8 == SYSCALL_GET_TASK_ID) {
        syscallReturnValue = __get_taskid();
    } else if (x8 == SYSCALL_GET_TIMESTAMP) {
        __get_timestamp();
    } else if (x8 == SYSCALL_ENABLE_IRQ) {
        __enable_irq();
    } else if (x8 == SYSCALL_CORE_TIMER_INIT) {
        __core_timer_init();
    } else if (x8 == SYSCALL_LOCAL_TIMER_INIT) {
        __local_timer_init();
    } else if (x8 == SYSCALL_EXEC) {
        do_exec(x0);
    } else if (x8 == SYSCALL_EXIT) {
        __exit(x0);
    } else if (x8 == SYSCALL_FORK) {
        syscallReturnValue = __fork();
    } else if (x8 == SYSCALL_UART_READ) {
        syscallReturnValue = uart_getc();
    } else if (x8 == SYSCALL_UART_WRITE) {
        uart_puts(x0);
    }
    trapframe->regs[0] = syscallReturnValue;
}

void exception_handler(unsigned long sp)
{
    unsigned int el_level;
    unsigned int esr, elr, spsr, far;
    unsigned int ec, iss, retaddr;
    unsigned int sp_el0, elr_el1, spsr_el1;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el_level));
    if (el_level == 0x4) {
        asm volatile ("mrs %0, esr_el1" : "=r" (esr));
        asm volatile ("mrs %0, elr_el1" : "=r" (elr));
        asm volatile ("mrs %0, spsr_el1" : "=r" (spsr));
        asm volatile ("mrs %0, far_el1" : "=r" (far));
    } else if (el_level == 0x8) {
        asm volatile ("mrs %0, esr_el2" : "=r" (esr));
        asm volatile ("mrs %0, elr_el2" : "=r" (elr));
        asm volatile ("mrs %0, spsr_el2" : "=r" (spsr));
        asm volatile ("mrs %0, far_el2" : "=r" (far));
    }
    ec = esr >> (32-6);
    iss = esr & (0xffffff);
    retaddr = elr;

    asm volatile ("mrs %0, sp_el0" : "=r" (sp_el0));
    asm volatile ("mrs %0, elr_el1" : "=r" (elr_el1));
    asm volatile ("mrs %0, spsr_el1" : "=r" (spsr_el1));

    // Set trapframe
    Task *task = get_current();
    task->userContext.sp_el0 = sp_el0;
    task->userContext.elr_el1 = elr_el1;
    task->userContext.spsr_el1 = spsr_el1;
    task->trapframe = sp;

    // SVC
    if ((esr>>26) == 0b010101) {
        el0_svc_handler(sp);
    }

    // uart_puts("[exp] Exception Level: 0x");
    // uart_print_hex(el_level);
    // uart_puts("\n[exp] Exception Class: 0x");
    // uart_print_hex(ec);
    // uart_puts("\n[exp] Instruction Specific syndrome: 0x");
    // uart_print_hex(iss);
    // uart_puts("\n[exp] Exception return address: 0x");
    // uart_print_hex(retaddr);
    // uart_puts("\n\n");
}