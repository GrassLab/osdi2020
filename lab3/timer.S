.section ".text"

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global core_timer_enable
core_timer_enable:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  // delay a second
  mrs x0, CNTFRQ_EL0
  msr CNTP_TVAL_EL0, x0
  
  // enable timer
  mov x0, #1
  msr cntp_ctl_el0, x0 
  
  // enable timer interrupt
  mov x0, #2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] 

  ldp x29, x30, [sp], #16
  ret

.global core_timer_disable
core_timer_disable:
  // disable timer
  mov x0, #0
  msr cntp_ctl_el0, x0

  // disable timer interrupt
  mov x0, #0
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] 

  ret

#define EXPIRE_PERIOD 0xfffffff
.global core_timer_handler
core_timer_handler:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  // delay a second
  mrs x0, CNTFRQ_EL0
  msr CNTP_TVAL_EL0, x0

  ldp x29, x30, [sp], #16
  ret

  