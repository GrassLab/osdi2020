#include "include/uart.h"
#include "include/utils.h"
#include "include/reboot.h"
#include "include/string.h"
#include "include/mbox.h"
#include "include/framebuffer.h"
#include "include/irq.h"
#include "include/timer.h"
#include "include/sys.h"
#include "include/fork.h"
#include "include/mm.h"
#include "include/scheduler.h"
#include "include/printf.h"

void get_board_revision_info(){
  mbox[0] = 7 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_BOARD_REVISION; // tag identifier
  mbox[3] = 4; // response value buffer's length.
  mbox[4] = 0; // request value buffer's length
  mbox[5] = 0; // value buffer
  // tags end
  mbox[6] = END_TAG;

  if(mbox_call(8)){
	 uart_send_string("### Board Revision:"); 
 	 uart_hex(mbox[5]); // it should be 0xa020d3 for rpi3 b+
  	 uart_send_string("\r\n");
  }
  else{
  	uart_send_string("Unable to query\r\n");
  }
}


void get_VC_core_base_addr(){
  mbox[0] = 8 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_VC_MEMORY; // tag identifier
  mbox[3] = 8; // response value buffer's length.
  mbox[4] = 0; // request value buffer's length
  mbox[5] = 0; // value buffer
  mbox[6] = 0;
  // tags end
  mbox[7] = END_TAG;

  if(mbox_call(8)){
	 uart_send_string("### VC core base address:"); 
	 uart_hex(mbox[5]); //base address in bytes:3B400000
	 uart_send_string(", size ");
	 uart_hex(mbox[6]); //size:4C00000
	 uart_send_string("\r\n");
  }
  else{
  	uart_send_string("Unable to query\n");
  }
}

void foo(){
  int tmp = 5;
  printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
  exit(0);
}

void idle(){
  while(1){
    	schedule();
    	delay(100000);
  }
  printf("Test finished\n");
  while(1);
}


void mytest(){
	/* uart_write test
	char buffer[64]="Word in buffer\r\n";
	int success_write;
	success_write = uart_write(buffer,sizeof(buffer));
	printf("Write byte: %d\r\n",success_write);*/

	/* uart_read test
	char buffer[4];
	int success_read;
	printf(">>");
	success_read = uart_read(buffer,sizeof(buffer));
	printf("\r\nRead byte %d: ", success_read);
	for(int i=0;i<sizeof(buffer);i++)
		printf("%c",buffer[i]);
	printf("\r\n");*/

	fork();
	int cnt = 1;	
	//printf("Doing: %d, and cnt now %d\r\n",get_taskid(),cnt);
	printf("cnt at 0x%x \r\n",&cnt);
	while(cnt < 10) {
		printf("Task id: %d, cnt addr: 0x%x, value: %d\n", get_taskid(), &cnt, cnt);
		delay(100000000);
		++cnt;
	}
	printf("\r\n");
	exit(0);
}

void test() {	
	printf("Before fork SP: 0x%x\r\n",get_SP());
  	int cnt = 1; //share?
	if (fork() == 0) {
    		//fork();
    		//delay(100000);
		
    		fork();
		printf("SP: 0x%x\r\n",get_SP());
		int cnt = 1;	
		printf("Doing: %d, and cnt now %d\r\n",get_taskid(),cnt);
    		printf("cnt at 0x%x \r\n",&cnt);
		while(cnt < 10) {
      			printf("Task id: %d, cnt addr: 0x%x, value: %d\n", get_taskid(), &cnt, cnt);
      			delay(100000);
      			++cnt;
    		}
   	 	exit(0);
    		printf("Should not be printed\n");
  	} else {
		
    		printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
    		exec(foo);
  	}
	exit(0);	
}

void kernel_process(){	
	int err = do_exec(test);
    	if (err < 0){
        	printf("Error while moving process to user mode\r\n");
    	}
}

void zombie_reaper(){
	while(1){
		struct task_struct *p;
		for (int i=0; i < NR_TASKS;i++){
			p = task[i];
			if(p && p->state==TASK_ZOMBIE){
				//reclaim the resource
				// 1. pid
				free_pid(i);
				// 2.kernel_stack(memory)	
				free_page((unsigned long)p);
				task[i] = 0;
			}
		}
	}
}


void kernel_main(void)
{	
    uart_init();  
    init_printf(0, putc);
    
    printf("Hello, world!\r\n");
 
    enable_irq();        //clear PSTATE.DAIF
    core_timer_enable(); //enable core timer
   
    //get hardware information by mailbox
    get_board_revision_info();
    get_VC_core_base_addr();
 
    init_runQ(); 
    init_idle_task(task[0]); // must init 'current' as idle task first 
   

    // Here init a task being zombie reaper
    int res = privilege_task_create(zombie_reaper);
    if (res < 0) {
        	printf("error while starting process");
        	return;
    }

    for(int num=0;num<1;num++){ 
    	int res = privilege_task_create(kernel_process);
    	if (res < 0) {
        	printf("error while starting process");
        	return;
    	}
    }	
    
    idle();
}
