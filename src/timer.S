#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xffffff

  .global core_timer_enable
  .globl enable_irq
  .globl disable_irq

enable_irq:
  msr daifclr,#2
  ret

disable_irq:
  msr   daifset, #2
  msr daifclr,#2

core_timer_enable:

  mov x1, x0
  mov x0, 1
  msr cntp_ctl_el0, x0 // enable timer
  mov x0, x1
  msr cntp_tval_el0, x0 // set expired time
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt

  msr daifclr, #2
  ret

