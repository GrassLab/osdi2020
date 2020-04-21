#define CORE0_TIMER_IRQ_CTRL 	0x40000040

.extern local_timer_enable
.global core_timer_enable
core_timer_enable:
  	//core timer
	mov x0, #1
  	msr cntp_ctl_el0, x0 	// enable timer
  	mov x0, #0xffffff
 	msr cntp_tval_el0, x0 	// set expired time
  	mov x0, #2
  	ldr x1, =CORE0_TIMER_IRQ_CTRL
  	str x0, [x1] // enable timer interrupt
	msr daifclr, #0xf		//enable interrupt
	//side timer
	bl side_timer_enable
	ret

.extern count_tick
.global timer_context
timer_context:
	bl count_tick
	eret

	
