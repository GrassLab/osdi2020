/*
 * arm core timer
 */

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffffff

.global core_timer_enable
core_timer_enable:
	mov x0, 1
	
	// CNTP_CTL_EL0: Control register for the EL1 physical timer. 
	msr cntp_ctl_el0, x0 // enable timer
	//mov x0, EXPIRE_PERIOD
	
	// CNTFRQ_EL0: Reports the frequency of the system timer
	mrs	x0, cntfrq_el0
	mov x0, x0, LSL 2
	// CNTP_TVAL_EL0: Holds the timer value for the EL1 physical timer.
	msr cntp_tval_el0, x0 // set expired time
	mov x0, 2	// bit 1

	// Core 0 Timers interrupt control
	// bit 1: nCNTPNSIRQ IRQ control.
	ldr x1, =CORE0_TIMER_IRQ_CTRL
	str x0, [x1] // enable timer interrupt
	
	ret

.global core_timer_handler
core_timer_handler:
	// CNTFRQ_EL0: Reports the frequency of the system timer
	mrs x0, cntfrq_el0
	
	// CNTP_TVAL_EL0: Holds the timer value for the EL1 physical timer.
	msr cntp_tval_el0, x0
	
	ret
