#define CORE0_TIMER_IRQ_CTRL 0x40000040

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


// #define EXPIRE_PERIOD 0x300000
// .global sched_core_timer_handler
// sched_core_timer_handler:
//   mov x0, EXPIRE_PERIOD
//   msr cntp_tval_el0, x0
//   bl  timer_tick
//   ret

.global core_timer_handler
core_timer_handler:
  mrs x0, cntfrq_el0
  msr cntp_tval_el0, x0
  ret

#define EXPIRE_PERIOD 0x8f00000
.global bh_core_timer_handler
bh_core_timer_handler:
  mov x0, EXPIRE_PERIOD
  msr cntp_tval_el0, x0
  ret
