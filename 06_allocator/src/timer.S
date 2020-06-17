#include "timer.h"

.global enable_core_timer
enable_core_timer:
  mov x0, 1
  msr cntp_ctl_el0, x0 // enable timer
  mov x0, EXPIRE_PERIOD
  msr cntp_tval_el0, x0 // set expired time
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt
  ret 

.global disable_core_timer
disable_core_timer:
  mov x0, 0
  msr cntp_ctl_el0, x0
  mov x0, 0
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1]
  ret 
