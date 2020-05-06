#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include "tools.h"
#include "uart.h"
#include "mbox.h"
#include "peripherals.h"
#include "gpio.h"
#include "utils.h"
#include "lfb.h"
#include "printf.h"
#include "irq.h"
#include "timer.h"
#include "entry.h"
#include "multitask.h"

extern unsigned char __bss_start;
extern unsigned char __bss_end;

extern struct task_struct * task_pool[MAX_THREAD_NUM];

void schedule_loop(){

	while(1){
		schedule();
	}
}
int get_taskid(){
	int pid;
	//asm volatile("mov x0, %0" ::"r"(&pid));
	uart_puts("Q1");
	asm volatile("mov x8, #2");	// #2 for get_taskid
	asm volatile("svc #0");
	asm volatile("mov %0, x0" : "=r"(pid) :);
	return pid;
}
void exec(unsigned long func){
	asm volatile("mov x0, %0" ::"r"(func));
	asm volatile("mov x8, #3");	// #3 for exec
	asm volatile("svc #0");
}
int fork(){
	int ret;
	asm volatile("mov x0, %0" ::"r"(&ret));
	asm volatile("mov x8, #4");	// #4 for fork
	asm volatile("svc #0");
	return ret;
}
void exit(int ret){
	asm volatile("mov x0, %0" ::"r"(&ret));
	asm volatile("mov x8, #5");	// #5 for exit
	asm volatile("svc #0");
}
char sys_uart_read(){
	char c;
	asm volatile("mov x0, %0" ::"r"(&c));
	asm volatile("mov x8, #6");	// #6 for sys_uart_read
	asm volatile("svc #0");
}
void sys_uart_write(int c){
	asm volatile("mov x0, %0" ::"r"(c));
	asm volatile("mov x8, #7");	// #7 for sys_uart_write
	asm volatile("svc #0");
}

void bar1(){
	uart_send('Q');
}
void bar2(){
	uart_send('W');
}
void user_task_3(){
	while(1){
		uart_send('E');
		// delay
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_user_task_3: subs  x0, x0, #1\n"
            "bne   loop_delay_user_task_3\n");
	}
}

void user_task_4(){
	while(1){
		uart_send('R');
		// delay
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_user_task_4: subs  x0, x0, #1\n"
            "bne   loop_delay_user_task_4\n");
	}
}

void user_syscall_test(){
	int pid = fork();
	char child_pid_string[100];
	char parent_pid_string[100];
	if(pid == 0){
		itoa(pid, child_pid_string, 10);
		uart_puts("PID: ");
		uart_puts(child_pid_string);
		uart_puts("\n");
		exec((unsigned long)user_task_4);
	}else{
		itoa(pid, parent_pid_string, 10);
		uart_puts("PID: ");
		uart_puts(parent_pid_string);
		uart_puts("\n");
		exec((unsigned long)bar1);
		exit(0);
	}
}

void task_1(){
	uart_puts("task_1 start\n");
	char pid_string[100];
	uart_puts("ready to get taskid\n");
	int pid = get_taskid();
	uart_puts("get taskid done\n");
	itoa(pid, pid_string, 10);
	uart_puts("task id: ");
	uart_puts(pid_string);
	uart_puts("\n");
	while(1){
		uart_puts("task1_loop\n");
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_task_1: subs  x0, x0, #1\n"
            "bne   loop_delay_task_1\n");
	}
}

void task_2(){
	do_exec((unsigned long)user_syscall_test);
	while(check_reschedule()){
		schedule();
	}
}

void task_3(){
	do_exec((unsigned long)user_task_3);
	while(check_reschedule()){
		schedule();
	}
}

void foo(){
	int tmp = 5;
	unsigned long tmp_address = (unsigned long)&tmp;
	int pid = get_taskid();
	char tmp_string[100];
	char tmp_address_string[100];
	char pid_string[100];
	itoa(tmp, tmp_string, 10);
	itoa(tmp_address, tmp_address_string, 10);
	itoa(pid, pid_string, 10);
	uart_puts("Task ");
	uart_puts(pid_string);
	uart_puts(" after exec, tmp address 0x");
	uart_puts(tmp_address_string);
	uart_puts(", tmp value ");
	uart_puts(tmp_string);
	uart_puts("\n");

	exit(0);
}

void test(){

	int child_pid;
	int parent_pid;
	char child_pid_string[100];
	char child_cnt_string[100];
	char parent_pid_string[100];
	char parent_cnt_string[100];
	
	register int cnt = 1;
    if (fork() == 0){
	
        fork();
        //delay(10000);
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_test: subs  x0, x0, #1\n"
            "bne   loop_delay_test\n");
        fork();
        while (cnt < 10){
			
			child_pid = get_taskid();
			itoa(child_pid, child_pid_string, 10);
			itoa(cnt, child_cnt_string, 10);
			uart_puts("Task id: ");
			uart_puts(child_pid_string);
			uart_puts(", cnt: ");
			uart_puts(child_cnt_string);
			uart_puts("\n");

			// delay
			asm volatile(
            	"mov  x0, #0xfffff\n"
            	"loop_delay_test_second: subs  x0, x0, #1\n"
            	"bne   loop_delay_test_second\n");



            //delay(1000000);
            ++cnt;
        }
        exit(0);
		uart_puts("Should not print this\n");
    }else{
		parent_pid = get_taskid();
		itoa(parent_pid, parent_pid_string, 10);
		itoa(cnt, parent_cnt_string, 10);
		uart_puts("Task ");
		uart_puts(parent_pid_string);
		uart_puts(" before exec, cnt value ");
		uart_puts(parent_cnt_string);
		uart_puts("\n");
        exec((unsigned long)foo);
    }
}

void user_test(){
    do_exec((unsigned long)test);
}

void foo1(){
	while(1){
		uart_send('1');
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_foo1: subs  x0, x0, #1\n"
            "bne   loop_delay_foo1\n");
	}
}
void foo2(){
	while(1){
		uart_send('2');
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_foo2: subs  x0, x0, #1\n"
            "bne   loop_delay_foo2\n");
	}
}
void foo3(){
	while(1){
		uart_send('3');
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_foo3: subs  x0, x0, #1\n"
            "bne   loop_delay_foo3\n");
	}
}
void foo4(){
	while(1){
		uart_send('4');
		asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_foo4: subs  x0, x0, #1\n"
            "bne   loop_delay_foo4\n");
	}
}
void main()
{
	// UART0 initialization
    uart_init();
	uart_puts("test1\n");
	// init (pid = 0)
	set_current(0);
	uart_puts("test2\n");
	task_pool[copy_process(FLAG_KERNEL, (unsigned long)schedule_loop, 0, 0)]->state = TASK_IDLE;
	uart_puts("test3\n");
	// create tasks
	copy_process(FLAG_KERNEL, (unsigned long)task_1, 0, 0);
	//copy_process(FLAG_KERNEL, (unsigned long)foo1, 0, 0);
	uart_puts("test4\n");
	//copy_process(FLAG_KERNEL, (unsigned long)task_2, 0, 0);
	//copy_process(FLAG_KERNEL, (unsigned long)foo2, 0, 0);
	uart_puts("test5\n");
	copy_process(FLAG_KERNEL, (unsigned long)task_3, 0, 0);
	//copy_process(FLAG_KERNEL, (unsigned long)foo3, 0, 0);
	uart_puts("test6\n");
	//copy_process(FLAG_KERNEL, (unsigned long)user_test, 0, 0);
	//copy_process(FLAG_KERNEL, (unsigned long)foo4, 0, 0);
	uart_puts("test7\n");

	// enable core timer
	asm volatile("sub sp, sp, 8");
	asm volatile("str x8, [sp, #8]");
	asm volatile("mov x8, #1");	// #1
	asm volatile("svc #0");	// arm system call
	asm volatile("ldr x8, [sp, #8]");
	asm volatile("add sp, sp, 8");
	
	uart_puts("test8\n");

	while(1);
}
