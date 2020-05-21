#include "timer.h"
#include "sys.h"
#include "mini_uart.h"
#include "irq.h"
#include "fork.h"
#include "mm.h"
#include "sched.h"
#include "../include/printf.h"
#define ONE_SEC 1000000
#define MAN_SEC 10000000
//#define REQ_1_2
//#define REQ_3_4
#define READ
//#define ELE_1

#ifdef REQ_1_2
void foo(){
  while(1) {
    printf("Task id: %d\r\n", current->task_id);
    delay(ONE_SEC);
    schedule();
  }
}
void idle(){
  while(1){
    schedule();
    delay(ONE_SEC);
  }
}
#endif
#ifdef REQ_3_4
void foo(){
  int tmp = 5;
  printf("Task %d after exec, tmp address 0x%x, tmp value %d\r\n", get_taskid(), &tmp, tmp);
  exit(TASK_ZOMBIE);
}

void test() {
  int cnt = 1;
  if (fork()== 0) {
    fork();
    delay(ONE_SEC);
    fork();
    while(cnt < 10) {
		  printf("Task id: %d, cnt address 0x%x, cnt value %d\r\n", get_taskid(), &cnt, cnt);
    	delay(ONE_SEC);
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

void usertest()
{
	do_exec((unsigned long)test);
}
#endif

#ifdef READ
void test() {
  int cnt;
  char buff[50];
  printf("delay...\r\n");
  delay(MAN_SEC);
  cnt = sync_call_uart_read(buff, 50);
  printf("read size %d, content: %s\r\n", cnt, buff);
  exit(TASK_ZOMBIE);
}

void usertest()
{
	do_exec((unsigned long)test);
}

#endif


#ifdef ELE_1
void test_kill() {
  unsigned long pid;
  pid = fork();
  if (pid == 0) {
    while (1) {
      printf("Hello from child\r\n");
      delay(ONE_SEC);
    }
  }
  else {
    delay(MAN_SEC);
    kill(pid);
    printf("Killed child\r\n");
  }
  exit(TASK_ZOMBIE); 
}

void usertest()
{
	do_exec((unsigned long)test_kill);
}

#endif

#ifndef REQ_1_2
void idle(){
  while(1){
    if(nr_tasks == 1) {
      break;
    }
    schedule();
    delay(ONE_SEC);
    enable_irq();
  }
  uart_send_string("Test finished\r\n");
  disable_irq();
  while(1);
}
#endif

void kernel_main(void)
{	
  init_printf(0, putc);
  while (1) {
		if (uart_recv()) {
            break;
		}
	}
  uart_send_string("uart_init\r\n");

  #ifdef  REQ_1_2
  for(int i = 0; i < 3; ++i) { // N should > 2
    privilege_task_create(PF_KTHREAD, foo, 0, 0);
  }
  enable_core_timer(); //enable timer
  enable_irq();
  #endif

  #ifndef REQ_1_2
  enable_core_timer(); //enable timer
  int res = privilege_task_create(PF_KTHREAD, usertest, 0, 0); //kernel init task fork the process
  enable_irq();
  #endif

  idle();
}
