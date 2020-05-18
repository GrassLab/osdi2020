#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "exc.h"
#include "syscall.h"
#include "task.h"
#include "syscall_io.h"

#define Idle_size 0x007ffff;

int syscall();
void exec(void(*func)());
int fork();

void exit(int x)
{
    syscall(9, x);
}


int get_taskid()
{
	return (int)syscall(4);
}

void delay()
{
	unsigned long long size = Idle_size;
	while(size--){asm volatile("nop");}
}

void idle()
{
	while(1){
		if((runqueue.tail - runqueue.head) == 1)
		{
			break;
		}
		//uart_puts("idle now\r\n");
		//task_schedule();
		asm volatile("mov x0, 3\r\nsvc #0");
		delay(1000000);
	}
	my_printf("Test finished\n");
  	while(1);

}

void foo()
{
	int tmp = 5;
	my_printf("Task %d after exec, tmp address 0x%x, tmp value %d\r\n", get_taskid(), &tmp, tmp);
	exit(0);
}

void utask6()
{
	int cnt=1;
	if(fork() == 0)
	{
		fork();
		delay();
		fork();
		while(cnt < 10) {
			my_printf("C Task id: %d, cnt address 0x%x, cnt: %d\r\n", get_taskid(), &cnt,cnt);
			delay(100000);
			++cnt;
    	}
		exit(0);
		my_printf("Should not be printed\r\n");
	}
	else{
		my_printf("Task %d before exec, cnt address 0x%x, cnt value %d\r\n", get_taskid(), &cnt, cnt);
    	exec(foo);
	}
	
}

void task6()
{
	toggle_privilege();
	do_exec(utask6, 0);
}



void kernel_init()
{
	unsigned long long int sp;
	uart_init();
	asm volatile("mov %0, sp":"=r"(sp)::);
//aaaaa:
	//uart_hex((unsigned long long)&&aaaaa);
	//my_printf("hahaha\r\n");
	uart_puts("hahaha\r\n");
	while(1);
	_global_coretimer = 0;
	task_struct_init();
	
	int idleid = privilege_task_create(idle);
	privilege_task_create(task6);

	asm volatile("msr tpidr_el1, %0"::"r"(&task_pool[idleid]):);
	asm volatile("mov sp, %0"::"r"(task_pool[idleid].ksp):);

	core_time_enable();
	
	idle();

	
}

void exec(void(*func)())
{
	syscall(7, func);
}

int fork()
{
	return syscall(8);
}


