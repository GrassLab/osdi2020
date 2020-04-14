.section ".text.exception"

.global _enable_core_timer
_enable_core_timer:
  // enable timer
  mov x0, 1
  msr cntp_ctl_el0, x0

  // set expired time for triggering first interrupt faster
  bl _core_timer_handler

  // enable timer interrupt
  mov x0, 2
  ldr x1, =0x40000040 // core 0 timer irq control
  str x0, [x1]
  ret

.global _core_timer_handler
_core_timer_handler:
  mov x0, 0x3f00000 // expired after this # of ticks, this # is close to 1 sec
  msr cntp_tval_el0, x0
  ret
