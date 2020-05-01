#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"
#include "fork.h"
#include "mm.h"
#include "sched.h"
#include "../include/printf.h"
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
  printf("Task %d after exec, tmp address 0x%x, tmp value %d\r\n", get_taskid(), &tmp, tmp);
  exit(TASK_ZOMBIE);
}

void test_read() {
    char buff[100];
    sync_call_uart_read(buff, 100);
    printf("%s",buff);
    exit(TASK_ZOMBIE);
}

void test() {
  int cnt = 1;
  if (fork()== 0) {
    fork();
    delay(10000000);
    fork();
    while(cnt < 10) {
		  printf("Task id: %d, cnt: %d\r\n", get_taskid(), cnt);
    	delay(100000000);
    	++cnt;
    }
    exit(TASK_ZOMBIE);
	  printf("Should not be printed\n");
  }
  else {
	  printf("Task %d before exec, cnt address 0x%x, cnt value %d\r\n", get_taskid(), &cnt, cnt);
    exec(foo);
  }
  exit(TASK_ZOMBIE);
}

void test_kill() {
  unsigned long pid = fork();
  if (pid == 0) {
    while (1) {
      printf("Hello from child\r\n");
      delay(100000000);
    }
  }
  else {
    kill(pid);
    printf("Killed child\r\n");
  }
  exit(TASK_ZOMBIE); 
}

void usertest()
{
  //do_exec((unsigned long)test_read);
	do_exec((unsigned long)test);
  //do_exec((unsigned long)test_kill);
}

void idle(){
  while(1){
    if(nr_tasks == 1) {
      break;
    }
    schedule();
    delay(10000000);
  }
  uart_send_string("Test finished\r\n");
  while(1);
}

void kernel_main(void)
{	
  init_printf(0, putc);
  uart_recv();
  uart_send_string("uart_init\r\n");
  sync_call_time();
  enable_irq();
  int res = privilege_task_create(PF_KTHREAD, usertest, 0, 0); //kernel init task fork the process 
  idle();
}
