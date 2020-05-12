// arm core timer
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0x3FFFFFF
#define TIMER_EN        (1<<0)
#define TIMER_INTEN     (1<<1)

.global core_timer_enable
core_timer_enable:
    mov x0, TIMER_EN
    msr cntp_ctl_el0, x0 // enable timer
    ldr x0, =EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    mov x0, TIMER_INTEN
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1] // enable timer interrupt
    ret

.global core_timer_disable
core_timer_disable:
    mrs x0, cntp_ctl_el0
    ldr x1, =TIMER_EN
    eor x1, x1, x1 // not x1
    and x0, x0, x1
    msr cntp_ctl_el0, x0

    ldr x2, =CORE0_TIMER_IRQ_CTRL
    ldr x0, [x2]
    ldr x1, =TIMER_INTEN
    eor x1, x1, x1 // not x1
    and x0, x0, x1
    str x0, [x2] // enable timer interrupt
    ret



.global core_timer_handler
core_timer_handler:
    ldr x0, =EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    ret