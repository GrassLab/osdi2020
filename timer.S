#define CORE0_TIMER_IRQ_CTRL 	0x40000040
#define EXPIRE_PERIOD 			0xffffff

.global core_timer_enable
core_timer_enable:
  	mov x0, #1
  	msr cntp_ctl_el0, x0 	// enable timer
  	mov x0, EXPIRE_PERIOD
 	msr cntp_tval_el0, x0 	// set expired time
  	mov x0, #2
  	ldr x1, =CORE0_TIMER_IRQ_CTRL
  	str x0, [x1] // enable timer interrupt
	msr daifclr, #0xf		//enable interrupt
	ret

.extern count_tick
.global core_timer_context
core_timer_context:
  	mov x0, EXPIRE_PERIOD
  	msr cntp_tval_el0, x0
	bl count_tick
	eret
