int cntfrq = 0;

void timmer_init()
{
	/*
   * Get CNTFRQ_EL0
   */
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq)::);
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