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
#include "include/signal.h"
#include "include/queue.h"
#include "include/kernel.h"
#include "include/elf.h"
#include "include/pool.h"

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
				free_page(virtual_to_physical((unsigned long)p));
				task[i] = 0;
			}
		}
	}
}


void kernel_process(){
    unsigned long begin = (unsigned long)&_binary_user_img_start;
    unsigned long end = (unsigned long)&_binary_user_img_end;

    //unsigned long elf_start = (unsigned long)&_binary_user_elf_start;
    //unsigned long elf_end = (unsigned long)&_binary_user_elf_end;
    //elf_parser(elf_start);
    

    // Note: we naive assume that there's only one shell   
    int err = do_exec(begin, end - begin, 0x1000);
    if (err < 0){
        printf("Error while moving process to user mode\n\r");
    }
}

void mytest1(){
    //test case
    unsigned long p1;
    p1 = get_free_page(0);
    printf("the return address 0x%x \r\n\r\n",p1); 
    
    unsigned long p2;
    p2 = get_free_page(0);
    printf("the return address 0x%x \r\n\r\n",p2);
    
    unsigned long p3;
    p3 = get_free_page(0);
    printf("the return address 0x%x \r\n\r\n",p3);
    
    free_page(p2);
    free_page(p3); 
 
    unsigned long p4;
    p4 = get_free_page(7);
    printf("the return address 0x%x \r\n\r\n",p4);
}

void mytest2(){
    int pool_num = allocator_register(0x100);
    unsigned long test_ptr1;
    unsigned long test_ptr2;

    // allocate memory from memory pool 
    test_ptr1 = allocator_alloc(pool_num,KERNEL_ALLOC);
    test_ptr2 = allocator_alloc(pool_num,KERNEL_ALLOC);
    printf("the return address 0x%x\r\n",test_ptr1);
    printf("the return address 0x%x\r\n",test_ptr2);
    
    allocator_free(pool_num,test_ptr1);
    test_ptr1 = allocator_alloc(pool_num,KERNEL_ALLOC);
    printf("the return address 0x%x\r\n",test_ptr1);
    
    unsigned long test_ptr3;
    test_ptr3 = allocator_alloc(pool_num,KERNEL_ALLOC);
    printf("the return address 0x%x\r\n",test_ptr3);

    allocator_unregister(pool_num,KERNEL_ALLOC);
}

void mytest3(){
	int *a = (int *)kmalloc(sizeof(int)*10);
	a[5] = 3;
	printf("a[5]= %d at 0x%x\r\n",a[5],&a[5]);
}

void kernel_main(void)
{	
    irq_vector_init(); 
    
    uart_init();   
    printf("Hello, world!\r\n");
    
    //get hardware information by mailbox
    get_board_revision_info();
    get_VC_core_base_addr();
  
    init_priority_queue();
    init_idle_task(task[0]); // must init 'current' as idle task first 
    init_page_struct();

    enable_irq();        //clear PSTATE.DAIF
    core_timer_enable(); //enable core timer
    
    allocator_init(); 
    //mytest1();
    mytest2();
    //mytest3();
    

    // Here init a task being zombie reaper
    privilege_task_create(zombie_reaper,1);

    privilege_task_create(kernel_process, 1); 

    idle();  
}
