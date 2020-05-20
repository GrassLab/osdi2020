#define CORE0_TIMER_IRQ_CTRL (0x40000040 + 0xffff000000000000)


#define PERIOD 0x300000
.global sys_core_timer_enable
sys_core_timer_enable:
  mov x0, #1
  msr cntp_ctl_el0, x0 // enable timer

  mov x0, PERIOD
  msr cntp_tval_el0, x0

  mov x0, #2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt

  ret
