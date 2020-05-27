#include "include/uart.h"
#include "include/string.h"
#include "include/utils.h"
#include "include/timer.h"
#include "include/mm.h"
#include "include/scheduler.h"
#include "include/fork.h"
#include "include/irq.h"
#include "include/peripherals/uart.h"
#include "include/signal.h"
#include "include/queue.h"
#include "include/reboot.h"
#include "include/exc.h"
#include "include/kernel.h"

void exception_handler(unsigned long type,unsigned long esr, \
		unsigned long elr){
        
	switch(type){
		case 0:uart_send_string("\r\nSynchronous");break;
		case 1:uart_send_string("\r\nIRQ");break;
		case 2:uart_send_string("\r\nFIQ");break;	
		case 3:uart_send_string("\r\nSError");break;	
		case 4:uart_send_string("\r\nSynchronous at 0x400");break;	
		case 5:uart_send_string("\r\nIRQ at 0x480");break;
	}
	uart_send_string(":");


	// decode some of exception type 
    	switch(esr>>26) {
        	case 0b000000: uart_send_string("Unknown"); break;
		case 0b010101: uart_send_string("System call"); break;
       		case 0b100100: uart_send_string("Data abort, lower EL"); break;
                case 0b100101: uart_send_string("Data abort, same EL"); break;
		case 0b011000: uart_send_string("Exception from MSR, MRS, or System instruction execution in AArch64 state");break;
		case 0b111100: uart_send_string("BRK instruction execution in AArch64 state");break;			       
		case 0b100000: uart_send_string("Instruction Abort from a lower Exception level");break;
		case 0b100001: uart_send_string("Instruction Abort taken without a change in Exception level");break;
		
		default: uart_send_string("Unknown...?"); break;
        }
        
        if(esr>>26==0b100100 || esr>>26==0b100101 || esr>>26==0b100000  ||esr>>26==0b100001 ) {
        	uart_send_string(", ");
        	switch((esr>>2)&0x3) {
            		case 0: uart_send_string("Address size fault"); break;
            		case 1: uart_send_string("Translation fault"); break;
            		case 2: uart_send_string("Access flag fault"); break;
            		case 3: uart_send_string("Permission fault"); break;
        	}
        	switch(esr&0x3) {
            		case 0: uart_send_string(" at level 0"); break;
            		case 1: uart_send_string(" at level 1"); break;
            		case 2: uart_send_string(" at level 2"); break;
            		case 3: uart_send_string(" at level 3"); break;
        		}
    	}


	// elr: return address
        uart_send_string("\r\nException return address: 0x");
	uart_hex(elr>>32);
	uart_hex(elr);

	// EC[31:26]: Exception class
	uart_send_string("\r\nException class(EC): 0x");
	uart_hex(esr>>26);

        // ISS[24:0]: Instruction Specific Syndrome
        uart_send_string("\r\nInstruction specific syndrome (ISS): 0x");
	uart_hex(( ((unsigned int)esr)<<7)>>7);

	uart_send_string("\r\n");
}

// Now I just use no more than 6 argument
unsigned long el0_svc_handler(size_t arg0,size_t arg1,size_t arg2,size_t arg3,\
		size_t arg4,size_t arg5, size_t sys_call_num){
	enable_irq();

	switch(sys_call_num){
		// Core timer
		case CORE_TIMER:{
			core_timer_enable();
			return 0;
		}
		// DAIF information
		case DAIF:{
			unsigned int daif;
          		asm volatile ("mrs %0, daif" : "=r" (daif));
          		printf("DAIF is %x\r\n",daif);
			
			return 0;
		}
		// kill
		case SYS_KILL:{
			struct task_struct *p = task[arg0];
			if(p && p->signal.pending!=SIGKILL)
				p -> signal.pending = SIGKILL;

			else
				printf("@@@ Signal failed\r\n");
			return 0;
		}
		// fork
		case SYS_FORK:{
			return user_task_create();
		}
		// exec
		case SYS_EXEC:{
			return 0;
		 	//return do_exec((void *)arg0);      
		}
		// exit
		case SYS_EXIT:{
			exit_process();
			return 0;
		}
		// get task pid
		case SYS_GET_TASKID:{
			return current->pid;
		}
		//  uart write
		case SYS_UART_WRITE:{
			// Using blocking write for safety
			preempt_disable();	
			
			int success = 0;
			int ret = 0;
			
			for(unsigned int i=0; i<arg1;i++){
				ret = uart_send(((char*)arg0)[i]);
				if(ret==0)
					++success;
			}

			preempt_enable();
		 	return success;	
		}
		// uart read
		case SYS_UART_READ:{	 
		      char recv_char;
		      unsigned int i = 0;
		      int flag = 0;
		   
		      for(;i<arg1;i++){
		      		// put task in waitQ and wait
			        current->state = TASK_WAIT;
		      		priorityQ_push(&waitqueue,1,current->pid); 
				
				//recv and send
				recv_char = uart_recv();		
				uart_send(recv_char);

				if(recv_char =='\n' || recv_char == '\r'){
					flag = 1;
					break;
				}
				else{ 
					((char*)arg0)[i] = recv_char;
				}
			}
			
			while(flag==0){
		      		// put task in waitQ and wait
			        current->state = TASK_WAIT;
				priorityQ_push(&waitqueue,1,current->pid); 
				
				//recv and send
				recv_char = uart_recv();
				uart_send(recv_char);

				if(recv_char =='\n' || recv_char == '\r')
					break;
			}	
			
			// send "\r\n"
			uart_send('\r');
			uart_send('\n');
			return i;	
		}
		// user_printf: allow only one argument now
		case SYS_UART_PRINT:{
			printf((char *)arg0,arg1,arg2,arg3,arg4);
			return 0;
		}
		// reboot
		case SYS_REBOOT:{
			reset(10000);
			return 0;	
		}
		// delay
		case SYS_DELAY:{
			delay(arg0);
			return 0;
		}
		// remain page num
		case SYS_REMAIN_PAGE:{
			return remain_page;
		}
		case SYS_MMAP: {
			return (unsigned long)mmap((void *)arg0,arg1,arg2,arg3,(void *)arg4,arg5);	       
		}
		case SYS_WAIT: {
			return current->state = TASK_WAIT;
		}	
		case SYS_MALLOC: {
			return (unsigned long)mmap(NULL, arg0, PROT_READ|PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
		}
		case SYS_FREE:{
			unsigned long vir_addr = (arg0>>12)<<12;
			// note: we dont't clean vm_struct and page table now
			// so there are some problem leave to be solve
			// 1. Since page table not clean, even if user free A, 
			// he can still access it because page table not
			// clear, but if any one allocate that page, undefined behavior happened
			
			// 2. Since vm_struct not clean, so user will get another virtual address
			// for malloc, if the virtual address map to a previous allocate and free
			// page, page fault won't happened. And this mean two different virtual 
			// address will map to same physical address

			//free using page
			for(int i=0;i<current->mm.user_pages_count;i++){
				 if(vir_addr == current->mm.user_pages[i].vir_addr){
				 	free_page(current->mm.user_pages[i].phy_addr);
				 	return 0;
				 }
			} 
			
			return -1;	      
		}	 
	}
	// Not here if no bug happened!
	return -1;
}

