#include <stdint.h>

#include "io.h"
#include "page.h"
#include "syscall.h"
#include "task.h"
#include "timer.h"
#include "trap.h"
#include "uart.h"

extern void core_timer_enable();
void synchronize_handler(uint64_t esr, uint64_t elr,
                         struct trap_frame_t *trap_frame) {
    struct task_t *task = get_current();
    task->trap_frame = trap_frame;
    int iss = esr & ((1 << 24) - 1);
    uint64_t *pgd;
    asm volatile("mrs %0, ttbr1_el1" : "=r"(pgd));
    move_ttbr(pgd);
    int ec = esr >> 26;
    if (ec == 0b100101) {
        uint64_t far;
        asm volatile("mrs %0, FAR_EL1" : "=r"(far));
        print_s("Page fault at address: ");
        print_h(far);
        print_s("\n");
        print_s("Exception return address: 0x");
        print_h(elr);
        print_s("\n");
        do_kill(task->id, SIGKILL);
        schedule();
        return;
    }
    if (iss == 0) {
        print_s("Exception return address: 0x");
        print_h(elr);
        print_s("\n");
        print_s("Exception class: 0x");
        print_h(ec);
        print_s("\n");
        print_s("ISS: 0x");
        print_h(iss);
        print_s("\n");
        while (1)
            ;
    }
    if (iss == 1) {
        uint64_t syscall = trap_frame->x8;
        syscall_handler(syscall, trap_frame, elr);

        uint64_t *pgd = task->pgd;
        move_ttbr(pgd);
    } else if (iss == 2) {
        core_timer_enable();
    }
}

void set_aux() { *(ENABLE_IRQS_1) = AUX_IRQ; }

void irq_handler() {
    uint64_t *pgd;
    asm volatile("mrs %0, ttbr1_el1" : "=r"(pgd));
    move_ttbr(pgd);
    if ((*CORE0_IRQ_SOURCE) & (1 << 11)) {
        local_timer_handler();
    } else if ((*CORE0_IRQ_SOURCE) & (1 << 1)) {
        core_timer_handler();
    }
    struct task_t *task = get_current();
    pgd = task->pgd;
    move_ttbr(pgd);
}
