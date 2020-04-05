#define CORE0_TIMER_IRQ_CTRL 0x40000040
.globl core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0  // enable timer
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1]          // enable timer interrupt
    ret
