#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "exc.h"
#include "syscall.h"
#include "task.h"
#include "syscall_io.h"

#define Idle_size 0x06fffff;

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
		task_schedule();
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
	do_exec(utask6);
}



void kernel_init()
{
	//asm volatile("svc #0");
	//ustack_pool = kstack_pool + (4096*64);
	uart_init();
	/*unsigned long long int tmp;
	asm volatile("mov %0, sp":"=r"(tmp)::);
	uart_puts("spppppp: ");
	uart_hex(tmp);
    uart_puts("\r\n");*/
	_global_coretimer = 0;
	task_struct_init();
	
	int idleid = privilege_task_create(idle);
	//privilege_task_create(task1);
	//privilege_task_create(task2);
	//privilege_task_create(task3);
	//privilege_task_create(task4);
	privilege_task_create(task6);
	//unsigned long long addrrr = &task_pool[t1];
	
	
	//local_timer_init();
	
	asm volatile("msr tpidr_el1, %0"::"r"(&task_pool[idleid]):);
	asm volatile("mov sp, %0"::"r"(task_pool[idleid].ksp):);
	core_time_enable();
	idle();

	//task_schedule(0);
	//task1(); //80d60
}

void exec(void(*func)())
{
	syscall(7, func);
}

int fork()
{
	return syscall(8);
}


