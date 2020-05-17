.section ".text"
#define CORE0_TIMER_IRQ_CTRL (0xffffaaaa00000000+0x1000040)
.global sys_core_timer_enable
sys_core_timer_enable:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  bl delay
  mov x0, #1
  msr cntp_ctl_el0, x0 // enable timer
  mov x0, #2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt

  ldp x29, x30, [sp], #16
  ret

#define EXPIRE_PERIOD 0xfffffff
.global core_timer_handler
core_timer_handler:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  bl delay

  ldp x29, x30, [sp], #16
  ret

delay:
  // delay 15ms (1000ms >> 6)
  mrs x0, CNTFRQ_EL0
  lsr x0, x0, #6
  msr CNTP_TVAL_EL0, x0
  ret

