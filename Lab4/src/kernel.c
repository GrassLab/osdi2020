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
#include "include/signal.h"
#include "include/queue.h"

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

void foo_kernel(){
  	while(1) {
    		printf("Task id: %d\n", current -> pid);
    		delay(100000000);
    		schedule();
  	}
}


void foo(){
  	int tmp = 5;
  	printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
  	exit(0);
}

void idle(){
  	while(1){
 		//if(num_runnable_tasks() == 1) 
      		//	break;
    		schedule();
    		delay(100000);
  	}
  	printf("Test finished\n");
  	while(1);
}

void uart_test(){
	/* uart_write test
	char buffer[64]="Word in buffer\r\n";
	int success_write;
	success_write = uart_write(buffer,sizeof(buffer));
	printf("Write byte: %d\r\n",success_write);*/

	/* uart_read test */
	char buffer[16];
	int success_read;
	printf("### Task %d, call UART_READ\r\n",get_taskid());
	success_read = uart_read(buffer,sizeof(buffer));
	printf("\r\nRead byte %d: ", success_read);

	if(success_read>0){
		for(int i=0;i<success_read;i++)
			printf("%c",buffer[i]);
	}

	printf("\r\n");

	exit(0);
}

void mytest(){
	int cnt = 1;
	int pid = fork();
	
	while(cnt < 10) {
		printf("Task id: %d (priority %d), cnt addr: 0x%x, value: %d\r\n", get_taskid(),get_priority(), &cnt, cnt);
		delay(100000000);
		++cnt;
		if(pid>0&&cnt>7)
			kill(pid,SIGKILL);
	}
	exit(0);
}

void test() {
  	int cnt = 1;
  	if (fork() == 0) {
    		fork();
    		delay(100000);
    		fork();
    		while(cnt < 10) {
      			printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
      			delay(10000000);
      			++cnt;
   		 }
    		exit(0);
    		printf("Should not be printed\n");
  	} else {
    		printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
    		exec(foo);
 	 }
}

void user_test(){
  	do_exec(test);
}

void kernel_process_b(){
	int err = do_exec(uart_test);
    	if (err < 0){
        	printf("Error while moving process to user mode\r\n");
    	}
}

void kernel_process(){
	int err = do_exec(mytest);
    	if (err < 0){
        	printf("Error while moving process to user mode\r\n");
    	}
}

void zombie_reaper(){
	while(1){
		schedule(); // It's Ok to let others doing first
		delay(10000);
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
 
    //init_runQ(); 
    init_priority_queue(runqueue);
    init_idle_task(task[0]); // must init 'current' as idle task first 
   

    // Here init a task being zombie reaper
    int res = privilege_task_create(zombie_reaper,1);
    if (res < 0) {
        	printf("error while starting process");
        	return;
    }
    
    /* Test case 1: 
    for(int i = 0; i < 2; ++i) { // N should > 2
    	privilege_task_create(foo_kernel,1);
    }*/     

    /* Test case 2: 
    privilege_task_create(user_test,1);*/
    
    /* My test 
    for(int num=0;num<2;num++){ 
    	int res = privilege_task_create(kernel_process,num+1);

    	if (res < 0) {
        	printf("error while starting process");
        	return;
    	}
    }	*/
   	
    
    /* Test with uart */
    for(int num=0;num<2;num++){ 
    	int res = privilege_task_create(kernel_process_b,num+1);

    	if (res < 0) {
        	printf("error while starting process");
        	return;
    	}
     }
    
    idle();
}
