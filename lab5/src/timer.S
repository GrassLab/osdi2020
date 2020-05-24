#define __ASSEMBLY__
#include "timer.h"

#define TIMER_EN        (1<<0)
#define TIMER_INTEN     (1<<1)
#define EXPIRE_PERIOD 0xff

.globl core_timer_enable
core_timer_enable:
    mov x0, TIMER_EN
    msr cntp_ctl_el0, x0 // enable timer counter
    ldr x0, =EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    mov x0, TIMER_INTEN
    ldr x1, =CORE0_TIMER_IRQ_CTRL // send core timer interrupt to PIC(IRQ routing, "Quad-A7 control" pdf page 4)
    str x0, [x1] // enable timer interrupt
    ret


.globl clean_core_timer
clean_core_timer:
    mov x0, EXPIRE_PERIOD
    msr cntp_tval_el0, x0
    ret

.globl core_timer_disable
core_timer_disable:
   mov x0, 0
   msr cntp_ctl_el0, x0
   mov x0, 0
   ldr x1, =CORE0_TIMER_IRQ_CTRL
   str x0, [x1]          // disable timer interrupt
   ret

