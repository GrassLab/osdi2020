#include "fork.h"
#include "utils.h"
#include "sched.h"
#include "printf.h"
#include "mm.h"
#include "peripherals/mini_uart.h"

void sys_write(char * buf)
{
	printf(buf);
}

int sys_fork()
{
	return copy_process(0, 0, 0);
}

void sys_exit() 
{
	exit_process();
}

int sys_remain_page_num() 
{
	return 	get_remain_num();
}

int sys_task_id() 
{
	return current->task_id;
}

int sys_read(unsigned long buff_addr, int size) {
	int i = 0;
	char tmp;
	while(1) {
		if((get32(AUX_MU_LSR_REG)&0x01) == 0)
			continue;
		tmp = get32(AUX_MU_IO_REG)&0xFF;
		*((char *)buff_addr + i) = tmp;
		i++;
		if (i == size) break;
	}
	return i;
}

unsigned long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot) {
	return do_mmap(addr, current, len, prot);
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit, sys_remain_page_num, sys_task_id, sys_read, sys_mmap};
