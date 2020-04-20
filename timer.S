.section ".text"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0x01

.global _core_timer_enable
_core_timer_enable:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  // delay (x0) second
  // CNTFRQ_EL0 = 62500000
  // mrs x1, CNTFRQ_EL0
  mov x1, #62500
  mul x0, x0, x1
  msr CNTP_TVAL_EL0, x0

  mov x0, 1
  msr CNTP_CTL_EL0, x0 // enable timer
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt


  ldp x29, x30, [sp], #16
  ret


.global _core_timer_disable
_core_timer_disable:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  mov x0, 0
  msr CNTP_CTL_EL0, x0 // disable timer
  mov x0, 0
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt

  ldp x29, x30, [sp], #16
  ret




.global _core_timer_handler
_core_timer_handler:
  stp x29, x30, [sp, #-16]!
  mov x29, sp

  // delay (x0) second
  // CNTFRQ_EL0 = 62500000
  mov x1, #62500
  mul x0, x0, x1
  msr CNTP_TVAL_EL0, x0

  // for schedule
  // add schedule counter
  ldr x1, =schedule_cnt
  ldr x0, schedule_cnt
  add x0, x0, #1
  str x0, [x1]

/* 
  mov x0, EXPIRE_PERIOD
  msr CNTP_CTL_EL0, x0
  */

  ldp x29, x30, [sp], #16
  ret
/*

  mov x0, EXPIRE_PERIOD
  msr CNTP_CTL_EL0, x0
 */
