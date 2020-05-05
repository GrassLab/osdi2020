#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "exc.h"
#include "syscall.h"
#include "task.h"
#include "syscall_io.h"

#define Idle_size 0x007ffff;

void exec(void(*func)());

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

void utask1()
{
	//uart_puts("haha1\r\n");
	unsigned long long int sp;
	task *current_task;
here:
	asm volatile("mov %0,sp":"=r"(sp)::);
	uart_hex(sp);
	uart_puts("....1\r\n");
	delay();
	//context_switch(&task_pool[3]);
	goto here;
}

void utask2()
{
	//uart_puts("haha2\r\n");
	unsigned long long int sp;
	task *current_task;
here2:
	asm volatile("mov %0,sp":"=r"(sp)::);
	uart_hex(sp);
	uart_puts(" ....2\r\n");
	delay();
	//context_switch(&task_pool[2]);
	goto here2;
}

void task1()
{
	toggle_privilege();
	do_exec(utask1, 0);
}

void task2()
{
	toggle_privilege();
	do_exec(utask2, 0);
}


void kernel_init()
{
	
	uart_init();
	
	_global_coretimer = 0;
	task_struct_init();
	
	int idleid = privilege_task_create(idle);
	privilege_task_create(task6);
	privilege_task_create(task1);
	privilege_task_create(task2);

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


