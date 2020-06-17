#include "schedule.h"
#include "timer.h"
#include "uart.h"

unsigned int get_system_frequency()
{
	unsigned int res = 0;
	asm volatile("MRS %[result], CNTFRQ_EL0"
				 : [ result ] "=r"(res));
	return res;
}

unsigned int get_system_count()
{
	unsigned int res = 0;
	asm volatile("MRS %[result],  CNTPCT_EL0"
				 : [ result ] "=r"(res));
	return res;
}

void print_time()
{
	long countertimer = get_system_count();
	long frequency = get_system_frequency();
	countertimer = countertimer * 1000;
	long result = countertimer / frequency;
	printf("%d ms\n", result);
}

int syscall(unsigned int x0, unsigned int x1, unsigned int x2,
			unsigned int x3)
{
	int rev = 0;
	switch (x0)
	{
	case 0:
		core_timer_enable();
		printf("start timer\n");
		break;
	case 1:
		print_time();
		break;
	case 2:
		do_exec(x1);
		break;
	case 7:
		rev = do_fork();
		break;
	case 8:
		do_exit();
		break;
	case 9:
		rev = do_get_current();
		break;
	}
	return rev;
}

int exception_router(unsigned int x0, unsigned int x1, unsigned int x2,
					 unsigned int x3)
{
	int rev = 0;
	unsigned int elr_el1, esr_el1;
	unsigned int ec, iss;
	asm volatile("MRS %[result], elr_el1"
				 : [ result ] "=r"(elr_el1));
	asm volatile("MRS %[result], esr_el1"
				 : [ result ] "=r"(esr_el1));
	ec = esr_el1 >> 26;
	iss = esr_el1 & 0xffffff;
	if (ec == 0x15)
	{
		if (iss == 0)
		{
			rev = syscall(x0, x1, x2, x3);
		}
		else
		{
			printf("Exception return address 0x%x\r\n", elr_el1);
			printf("Exception class (EC) 0x%x\r\n", ec);
			printf("Instruction specific syndrome (ISS) 0x%x\r\n", iss);
		}
	}
	else
	{
		// printf("OMG\n");
		// printf("Exception class (EC) 0x%x\r\n", ec);
	}
	return rev;
}
