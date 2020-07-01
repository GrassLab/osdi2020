// #include "uart.h"
int cntfrq = 0;

void timmer_init()
{
	/*
   * Get CNTFRQ_EL0
   */
	asm volatile("mov	x0, #1\n\t"
		     "svc	#0\n\t"
		     "mov	%0, x0"
		     : "=r"(cntfrq)::);
	return;
	//asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq)::);
}

int getFrequency()
{
	return cntfrq;
}

unsigned long long getCurrentCount()
{
	unsigned long long cntpct;
	/*
   * Get CNTPCT_EL0
   */
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct)::);
	return cntpct;
}
float getCurrentTime()
{
	unsigned long long cntpct;
	/*
   * Get CNTPCT_EL0
   */
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct)::);
	return (float)cntpct / cntfrq;
}
void local_timer_init()
{
	asm volatile("mov	x0, #3\n\t"
		     "svc	#0\n\t");
}