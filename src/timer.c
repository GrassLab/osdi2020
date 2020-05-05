#include "io.h"
#include "reset.h"
#include "task.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xffff
#define LOCAL_TIMER_CONTROL 0x40000034

extern void disable_irq();
extern void enable_irq();

int system_time_c = 0;
int local_time_c = 0;
void core_timer_enable() {
    asm volatile(
        "mov x1, x0;"
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;"  // enable timer
        "mov x0, 0xfffff;"
        "msr cntp_tval_el0, x0;"  // set expired time
        "mov x0, 2;"
        "ldr x1, =0x40000040;"
        "str x0, [x1];"  // enable timer interrupt
        "msr daifclr, #2;");
}

void core_timer_handler() {
    system_time_c++;
    struct task_t* task = get_current();
    print_s("Timer interrupt\n");
    task->time++;
    uint64_t elr, sp_el0, spsr_el1;

    asm volatile("mov x0, 0x1");
    asm volatile("mrs x1, CNTFRQ_EL0");
    asm volatile("mul x0, x0, x1");
    asm volatile("msr cntp_tval_el0, x0");
    if (task->time >= 2) {
        asm volatile("mrs %0, elr_el1" : "=r"(elr));
        task->elr = elr;
        asm volatile("mrs %0, sp_el0" : "=r"(sp_el0));
        task->utask.sp = sp_el0;
        asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
        task->spsr = spsr_el1;
        /* task->reschedule = 1; */
        task->time = 0;
        asm volatile("ldr x0, =schedule");
        asm volatile("msr elr_el1, x0");
    }
    return;
}

#define LOCAL_TIMER_CONTROL_REG 0x40000034

void local_timer_init() {
    unsigned int flag = 0x30000000;  // enable timer and interrupt.
    unsigned int reload = 25000000;
    set(LOCAL_TIMER_CONTROL, flag | reload);
    asm volatile("msr daifclr, #2;");
}

#define LOCAL_TIMER_IRQ_CLR 0x40000038

void local_timer_handler() {
    local_time_c++;
    print_s("Local timer interrupt ");
    print_i(local_time_c);
    print_s("\n");
    set(LOCAL_TIMER_IRQ_CLR, 0xc0000000);  // clear interrupt and reload.
    int k = 214143141;
    asm volatile("msr daifclr, #2;");
    while (k--) {
        asm volatile("nop");
    }
}
