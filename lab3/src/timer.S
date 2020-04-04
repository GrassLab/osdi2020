#define CORE0_TIMER_IRQ_CTRL 0x40000040
.globl core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cnthp_ctl_el2, x0  // enable timer
    mov x0, 4
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1]          // enable timer interrupt
    ret
