#include "include/uart.h"
#include "include/string.h"
#include "include/utils.h"
#include "include/timer.h"
#include "include/mm.h"
#include "include/scheduler.h"
#include "include/fork.h"
#include "include/printf.h"
#include "include/irq.h"
#include "include/peripherals/uart.h"
#include "include/signal.h"
#include "include/queue.h"

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
		case 0b011000: uart_send_string("Exception from MSR, MRS, or System instruction execution in AArch64 state");
		case 0b111100: uart_send_string("BRK instruction execution in AArch64 state");break;			       
		case 0b100000: uart_send_string("Instruction Abort from a lower Exception level");break;
		case 0b100001: uart_send_string("Instruction Abort taken without a change in Exception level");break;
		
		default: uart_send_string("Unknown...?"); break;
        }
        
	// decode data abort cause
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

// Since my system call just need no more than two argument now
unsigned long el0_svc_handler(size_t arg0,size_t arg1,size_t sys_call_num){
	enable_irq();

	switch(sys_call_num){
		// Core timer
		case 0:{
			core_timer_enable();
			return 0;
		}
		// DAIF information
		case 1:{
			unsigned int daif;
          		asm volatile ("mrs %0, daif" : "=r" (daif));
          		printf("DAIF is %x\r\n",daif);
			
			return 0;
		}
		// kill
		case 2:{
			struct task_struct *p = task[arg0];
			if(p && p->signal.pending!=SIGKILL)
				p -> signal.pending = SIGKILL;

			else
				printf("@@@ Signal failed\r\n");
			return 0;
		}
		// fork
		case 3:{
			return user_task_create();
		}
		// exec
		case 4:{
		 	//return do_exec((void *)arg0);      
		}
		// exit
		case 5:{
			exit_process();
			return 0;
		}
		// get task pid
		case 6:{
			return current->pid;
		}
		//  uart write
		case 7:{
			// Using blocking write for safety
			preempt_disable();	
			
			int success = 0;
			int ret = 0;
			printf("Uart write byte: %d\r\n",arg1);	
			
			for(int i=0; i<arg1;i++){
				ret = uart_send(((char*)arg0)[i]);
				if(ret==0)
					++success;
			}

			preempt_enable();
		 	return success;	
		}
		// uart read
		case 8:{	 
		      char recv_char;
		      int i = 0;
		      int flag = 0;
		      
		      preempt_disable();

		      for(;i<arg1;i++){
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
				//recv and send
				recv_char = uart_recv();
				uart_send(recv_char);

				if(recv_char =='\n' || recv_char == '\r')
					break;
			}	
			
			// send "\r\n"
			uart_send('\r');
			uart_send('\n');

			preempt_disable();
			return i;	
		}
		// get priority
		case 9:{
			return current->priority;
		}

	}
	// Not here if no bug happened!
	return -1;
}

