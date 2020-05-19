#include <stdint.h>

#include "io.h"
#include "task.h"
#include "timer.h"
#include "trap.h"
#include "uart.h"

extern void core_timer_enable();
void synchronize_handler(uint64_t esr, uint64_t elr,
                         struct trap_frame_t *trap_frame) {
    int iss = esr & ((1 << 24) - 1);
    /* int ec = esr >> 26; */
    if (iss == 0) {
        print_s("Exception return address: 0x");
        print_h(elr);
        print_s("\n");
        while (1)
            ;
    }
    if (iss == 1) {
        uint64_t syscall = trap_frame->x8;
        switch (syscall) {
            case 0:
                for (uint64_t i = 0; i < trap_frame->x1; i++) {
                    uart_send(*((char *)trap_frame->x0 + i));
                }
                break;
            case 1:
                for (uint64_t i = 0; i < trap_frame->x1; i++) {
                    *((char *)trap_frame->x0 + i) = uart_getc();
                }
                /* asm volatile("mov x0, %0" : "=r"(trap_frame->x0)); */
                break;
            case 2:
                do_exec((void (*)())trap_frame->x0);
                break;
            case 3:
                do_fork(elr);
                break;
            case 4:
                do_exit(trap_frame->x0);
                asm volatile("ldr x0, =schedule");
                asm volatile("msr elr_el1, x0");
                break;
            case 5:
                do_kill(trap_frame->x0, trap_frame->x1);
                break;
        }
    } else if (iss == 2) {
        core_timer_enable();
    }
}

void set_aux() { *(ENABLE_IRQS_1) = AUX_IRQ; }

void irq_handler() {
    if ((*CORE0_IRQ_SOURCE) & (1 << 11)) {
        local_timer_handler();
    } else if ((*CORE0_IRQ_SOURCE) & (1 << 1)) {
        core_timer_handler();
    }
}
