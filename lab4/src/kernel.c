#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"
#include "fork.h"
#include "mm.h"
#include "../include/sched.h"

void user_process2() {
	int k;
	uart_send_string("666666\r\n");
	for (int i = 0 ; i < 10000 ; i++) {
		//uart_send_string("666666\r\n");
		delay(100000000);
	}
	exit(TASK_ZOMBIE);
}

void user_process1(char *array) {
	// this process run at el0
	int k;
	exec(user_process2);
	exit(TASK_ZOMBIE);
}

void user_process() {
	//system call for clone process
	unsigned long i;
	uart_send_string("start fork the user process\r\n");
	i = fork();
	if (i == 0) {
		uart_send_string("fork return from child process\r\n");
	}
	else {
		uart_send_string("fork return from parent process\r\n");
		exec(user_process2);
	}
	exit(TASK_ZOMBIE);
}

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0' && *str2 != '\0') {
            return 1;
        }
		else if (*str1 != '\0' && *str2 == '\0') {
			return 1;
		}
		else if (*str1 == '\0' && *str2 == '\0') {
			return 0;
		}
        str1++;
        str2++;
    }
}

void foo(){
  int tmp = 5;
  uart_send_string("Task ");
  uart_send_int(get_taskid());
  uart_send_string(" after exec, tmp address ");
  uart_send_hex(&tmp);
  uart_send_int(tmp);
  uart_send_string("\r\n");
  exit(TASK_ZOMBIE);
}

void test() {
  int cnt = 1;
  uart_send_int(get_taskid());
  uart_send_string("\r\n");
  if (fork() == 0) {
    fork();
    delay(100000000);
    fork();
    while(cnt < 10) {
		uart_send_string("Task id: ");
		uart_send_int(get_taskid());
		uart_send_string(", cnt: ");
		uart_send_int(cnt);
		uart_send_string("\r\n");
    	delay(1000000000);
    	++cnt;
    }
    exit(TASK_ZOMBIE);
	uart_send_string("Should not be printed\r\n");
  }
  else {
	uart_send_string("Task ");
	uart_send_int(get_taskid());
	uart_send_string(" before exec, cnt address ");
	uart_send_hex(&cnt);
	uart_send_string(", cnt value ");
	uart_send_int(cnt);
	uart_send_string("\r\n");
    exec(foo);
  }
  exit(TASK_ZOMBIE);
}

void usertest()
{
	do_exec((unsigned long)test);
}

void idle(){
  while(1){
    if(nr_tasks == 1) {
      break;
    }
    schedule();
    delay(100000000);
  }
  uart_send_string("Test finished\r\n");
  while(1);
}

void kernel_main(void)
{	
	uart_recv();
	uart_send_string("uart_init\r\n");
	sync_call_time();
	enable_irq();
	int res = privilege_task_create(PF_KTHREAD, usertest, 0, 0); //kernel init task fork the process 
	if (res == -1) {
		uart_send_string("init kernel process fails to fork\r\n");
	}
	idle();
}
