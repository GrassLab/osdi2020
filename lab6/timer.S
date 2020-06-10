.type timer_enable_core_timer,%function
.global timer_enable_core_timer
timer_enable_core_timer:

  // Expire the timer first
  mov x0, #0xffffffffffffffff
  msr cntp_cval_el0, x0

  mov x0, #1 // Enable timer and clear imask
  msr cntp_ctl_el0, x0 // Control register for the EL1 physical timer
  mov x0, #2 // CNTPNSIRQ interrupt
  ldr x1, =0xffff000040000040 // CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt
  ret

.type timer_disable_core_timer,%function
.global timer_disable_core_timer
timer_disable_core_timer:
  eor x0, x0, x0 // Disable timer
  msr cntp_ctl_el0, x0 // Control register for the EL1 physical timer

.type timer_set_core_timer_approx_ms,%function
.global timer_set_core_timer_approx_ms
timer_set_core_timer_approx_ms:
  // divide 1024 to frequency, which is why it is "approximately" 1 ms
  // x0 is number of seconds
  mrs x1, cntfrq_el0
  lsr x1, x1, #10
  mul x0, x0, x1
  msr cntp_tval_el0, x0
  ret

.type timer_set_core_timer_sec,%function
.global timer_set_core_timer_sec
timer_set_core_timer_sec:
  // x0 is number of seconds
  mrs x1, cntfrq_el0
  mul x0, x0, x1
  msr cntp_tval_el0, x0
  ret

.type timer_expire_core_timer,%function
.global timer_expire_core_timer
timer_expire_core_timer:
  mov x0, #0xffffffffffffffff
  msr cntp_cval_el0, x0
  ret

