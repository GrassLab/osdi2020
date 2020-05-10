#include "include/uart.h"
#include "include/utils.h"
#include "include/reboot.h"
#include "include/string.h"
#include "include/mbox.h"
#include "include/framebuffer.h"
#include "include/irq.h"
#include "include/timer.h"
#include "include/fork.h"
#include "include/mm.h"
#include "include/scheduler.h"
#include "include/printf.h"
#include "include/signal.h"
#include "include/queue.h"
#include "include/kernel.h"
#include "include/user_lib.h"

int check_string(char * str){
	char* cmd_help = "help";
	char* cmd_hello = "hello";
	char* cmd_reboot = "reboot";

	if(strcmp(str,cmd_help)==0){
	// print all available commands
		uart_send_string("\r\nhello : print Hello World!\r\n");
		uart_send_string("help : help\r\n");
		uart_send_string("reboot: reboot rpi3\r\n");
	}
	else if(strcmp(str,cmd_hello)==0){
	// print hello
		uart_send_string("\r\nHello World!\r\n");
	}
	else if(strcmp(str,cmd_reboot)==0){
		uart_send_string("\r\nBooting......\r\n");
		reset(10000);
		return 1;	
	}
	else{
		uart_send_string("\r\nErr:command ");
		uart_send_string(str);
		uart_send_string(" not found, try <help>\r\n");
	}
	return 0;
}

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

void idle(){
  while(1){
    	schedule();
    	delay(100000);
  }
}


void kernel_process(){
    printf("Kernel process started. EL %d\r\n", get_el());

    unsigned long begin = (unsigned long)&_binary_user_img_start;
    unsigned long end = (unsigned long)&_binary_user_img_end;
    
    printf("begin 0x%x%x\r\n",begin>>32,begin);
    printf("end 0x%x%x\r\n",end>>32,end);
    printf("size: 0x%x\r\n",end-begin);

    int err = do_exec(begin, end - begin, 0);
    if (err < 0){
        printf("Error while moving process to user mode\n\r");
    }
}

void kernel_main(void)
{	
    irq_vector_init(); 
    
    uart_init();   
    init_printf(0, putc);
    printf("Hello, world!\r\n");
    
    enable_irq();        //clear PSTATE.DAIF
    //core_timer_enable(); //enable core timer
   
    //get hardware information by mailbox
    get_board_revision_info();
    get_VC_core_base_addr();
 
    //init_runQ(); 
    init_priority_queue();
    init_idle_task(task[0]); // must init 'current' as idle task first 
    init_page_struct();

    int res = privilege_task_create(kernel_process, 1); 

    if (res < 0) {
		printf("error while starting kernel process\r\n");
		while(1);
    }

    idle();  
}
