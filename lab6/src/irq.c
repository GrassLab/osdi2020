#include "entry.h"
#include "uart.h"
#include "sys.h"
#include "fork.h"
#include "pool.h"
char *entry_error_messages[] = {
	"SYNC_INVALID_ELxt",
	"IRQ_INVALID_ELxt",		
	"FIQ_INVALID_ELxt",		
	"ERROR_INVALID_ELxT",		

	"SYNC_INVALID_ELxh",		
	"IRQ_INVALID_ELxh",		
	"FIQ_INVALID_ELxh",		
	"ERROR_INVALID_ELxh",		

	"SYNC_INVALID_ELxm1_64",		
	"IRQ_INVALID_ELxm1_64",		
	"FIQ_INVALID_ELxm1_64",		
	"ERROR_INVALID_ELxm1_64",	

	"SYNC_INVALID_ELxm1_32",		
	"IRQ_INVALID_ELxm1_32",		
	"FIQ_INVALID_ELxm1_32",		
	"ERROR_INVALID_ELxm1_32"	
};


void show_invalid_entry_message(int type, unsigned long esr, unsigned long elr)
{
	unsigned long iss = esr & 0x1ffffff;

    //esr[31:26]
    uart_puts("EC: 0x");
    uart_hex((unsigned int)(esr>>26));
    
    //esr[24:0]
    uart_puts(", ISS: 0x");
	uart_hex((unsigned int)(iss));

    uart_puts(", return address: 0x");
    uart_hex((unsigned int)(elr));
    uart_puts("\n");
}

int sync_handler(unsigned long x0, unsigned long x1){
    unsigned long current_el;
    current_el = get_el();
/*    uart_puts("Current EL: ");
    uart_hex(current_el);
    uart_puts("\n");
*/
	unsigned long esr;
    unsigned long elr;
	switch (current_el)
    {
        case 0:
            break;
        case 1:
            asm volatile(
                "mrs %0, elr_el1;"
                "mrs %1, esr_el1;"
                : "=r"(elr), "=r"(esr));
            break;
        case 2:
            asm volatile(
                "mrs %0, elr_el2;"
                "mrs %1, esr_el2;"
                : "=r"(elr), "=r"(esr));
            break;
        case 3:
            uart_puts("Something wrong. Halt");
            while (1)
                ;
        break;
    }

	unsigned long iss = esr & 0x1ffffff;
/*
    //esr[31:26]
    uart_puts("EC: 0x");
    uart_hex((unsigned int)(esr>>26));
    
    //esr[24:0]
    uart_puts(", ISS: 0x");
	uart_hex((unsigned int)(iss));

    uart_puts(", return address: 0x");
    uart_hex((unsigned int)(elr));
    uart_puts("\n");
*/   	
	if (iss == 0x80)
    {

        uart_puts("System Call[");
        uart_hex(x0);
        uart_puts("]\n");
        switch (x0)
        {
        // arm core timer
        case 0x0:
            core_timer_enable();
            break;
        // not this syscall
        default:
            uart_puts("Cannot find this system call");

            while (1)
                ;
        }
    }else if ((esr>>26)==0x15){
        unsigned int val;
        asm volatile("uxtw %0, w8":"=r"(val)); 
        enable_irq();
        if(val == SYS_FORK_NUMBER){
            uart_puts("[fork]");
            return privilege_task_create(0,0,0);            
        }else if(val == SYS_EXIT_NUMBER){
            uart_puts("[exit]");
            exit_process(); 
        }else if(val == SYS_EXEC_NUMBER){
            uart_puts("[exec]");
            return do_exec(x0);
        }else if(val == SYS_UART_READ_NUMBER){
            uart_puts("[uart_read]");
       		char recv_char;
		  	int i = 0;
		  	int flag = 0;
		  
		  	preempt_disable();

		    for(;i < x1;i++){
				//recv and send
				recv_char = uart_getc();		
				uart_send(recv_char);

				if(recv_char =='\n' || recv_char == '\r'){
					flag = 1;
					break;
				}
				else{ 
					((char*)x0)[i] = recv_char;
				}
			}
			
			while(flag==0){
				//recv and send
				recv_char = uart_getc();
				uart_send(recv_char);

				if(recv_char =='\n' || recv_char == '\r')
					break;
			}	
			
			// send "\r\n"
			uart_send('\r');
			uart_send('\n');

			preempt_disable();
			return i;	 
        }else if(val == SYS_UART_WRITE_NUMBER){
            uart_puts("[uart_write]");
			preempt_disable();
			int success = 0;
			int ret = 0;
			for(int i = 0; i < x1 ;i++){
                if(((char*)x0)[i] =='\0')
                    break;
				ret = uart_send(((char*)x0)[i]);
				if(ret==1)
					++success;
			}
			preempt_enable(); 
			return success;	
        }else if(val == SYS_GET_TASKID_NUMBER){
            return current->id;
        }else if(val == SYS_REGISTER_ALLOCATOR){
            return register_allocator(x0);
        }else if(val == SYS_REQ_ALLOCATE){
            return allocate(x0);
        }else if(val == SYS_FREE_ALLOCATE){
            free(x0, x1);
        }
        disable_irq();
    }
    return 0;
}

extern int core_jiffies = 0;
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060
void irq_handler(void)
{
    core_timer_handler();
    core_jiffies += 1;
    //uart_puts("Core timer interrupt, jiffies ");
    uart_hex(core_jiffies);
    uart_puts("\n");

    timer_tick();
}


