/*
 * arm core timer
 */

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffffff

.global core_timer_enable
core_timer_enable:
	mov x0, 1
	msr cntp_ctl_el0, x0 // enable timer
	//mov x0, EXPIRE_PERIOD
	mrs	x0, cntfrq_el0
	msr cntp_tval_el0, x0 // set expired time
	mov x0, 2
	ldr x1, =CORE0_TIMER_IRQ_CTRL
	str x0, [x1] // enable timer interrupt
	ret

.global core_timer_handler
core_timer_handler:
	mrs x0, cntfrq_el0
	msr cntp_tval_el0, x0
	ret
