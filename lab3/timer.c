/*=================== arm local timer ===================*/
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)

void arm_local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 0x25000000;
  *LOCAL_TIMER_CONTROL_REG = (flag | reload);
}

void arm_local_timer_handler(){
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000;
}

void arm_local_timer_cancel(){
	*LOCAL_TIMER_CONTROL_REG = 0;
}

/*=================== arm core timer ===================*/
#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)

void core_timer_handler(){
	asm volatile(
		"mov 	x0, %[period];" //EXPIRE_PERIOD
		"msr 	cntp_tval_el0, x0;"
		:
		: [period] "r" (0x5ffffff)
		:
	);
}

void core_timer_enable(){
	core_timer_handler();
	asm volatile(
		"mov 	x0, 1;"
		"msr	cntp_ctl_el0, x0;"

		:::
	);

	*CORE0_TIMER_IRQ_CTRL = 0b10;
}

void core_timer_cancel(){
	*CORE0_TIMER_IRQ_CTRL = 0;
}