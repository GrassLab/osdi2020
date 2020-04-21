#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0x1FFFFFF

.global core_timer_enable
core_timer_enable:
  mov x0,xzr
  orr x0,x0, #1
  msr cntp_ctl_el0, x0 // enable timer

  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str x0, [x1] // enable timer interrupt(CNTPNSIRQ)
  
  // should we need this?? 
  mov x0, EXPIRE_PERIOD
  msr cntp_tval_el0,x0
  ret

.global core_timer_handler
core_timer_handler:
  mov x0, EXPIRE_PERIOD
  msr cntp_tval_el0,x0 
  
  sub sp,sp,#16
  str x30,[sp,#16*0] 
  bl timer_tick //call schedule in timer
  ldr x30,[sp,#16*0]
  add sp,sp,#16

  ret
