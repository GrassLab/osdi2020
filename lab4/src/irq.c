#include "utils.h"
#include "timer.h"
#include "entry.h"
#include "irq.h"
#include "config.h"
#include "thread.h"
#include "syscall.h"

unsigned int c = 0;
unsigned int local_timer_count = 0;
unsigned int core_timer_count = 0;
extern task_t* current;
extern check_reschedule();

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void enable_interrupt_controller()
{
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	_print("Type: ");
	uart_send_int(type);
	_print("\n");

	_print("Exception return address 0x");
	uart_hex(address);
	_print("\n");
	
	_print("Exception class (EC) 0x");
	uart_send_int(esr >> 26);
	_print("\n");

	_print("Instruction  specific syndrome (ISS) 0x");
	uart_hex(esr & 0xffffff);
	_print("\n");
}

unsigned int get_syscall_number(unsigned int trapframe){
	unsigned int x8 = *(unsigned int*)(trapframe+8*8);
	return x8;
}

void handle_el0_sync(unsigned int trapframe, unsigned long esr)
{
	printf("Exception class (EC) 0x%x \r\n",esr >> 26);
	printf("EInstruction  specific syndrome (ISS) 0x%x \r\n",esr & 0xffffff);
	int num = (esr & 0xffffff);
	if(num == 0){
		unsigned int c;
		unsigned int x8 = get_syscall_number(trapframe);
		switch(x8){
			case SYS_GET_TASKID:
				printf("x8 value is %d\n",x8);
				printf("[info] svc000 command \r\n");
				break;
			case SYS_UART_READ:
				c = uart_getc();
				printf("get input value is %d\n",c);
				break;
			case SYS_UART_WRITE:
				printf("x8 value is %d\n",x8);
            	break;
			case SYS_EXEC:
				// do_exec(x0);
				break;
			case SYS_FORK:
				do_fork();
				break;
			default:
            	printf("syscall not found\n");
            	break;
		}
	}
	if (num == 1){
		printf("[info] svc command \r\n");
	}
	else if(num == 2){
		printf("[info] enable timer interrupt \r\n");

		enable_interrupt_controller();
		local_timer_init();

		unsigned int cntfrq;
        unsigned int val;
		cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);  
        val = read_cntp_tval();    
		
		core_timer_enable();
	}
	else if(num == 3){
		disable_all_timer();
	}
	else if(num == 4){
		timestamp_handler();
	}
	check_reschedule();

}

void timestamp_handler(){
	unsigned int time, timer_counter, timer_freq;
	char buf[10];
    _memset(buf,'\0',10);
	asm volatile("mrs %0, cntpct_el0": "=r"(timer_counter)::); 
	asm volatile("mrs %0, cntfrq_el0": "=r"(timer_freq)::);
    time = timer_counter / (timer_freq / 100000U);
	
	_unsign_arr_to_digit((time/100000U), buf, 5);
	uart_send('[');
	uart_puts(buf); 
	uart_send('.');
	_unsign_arr_to_digit(time%100000U, buf, 5);
	uart_puts(buf); 
	uart_send(']');
	uart_puts("\n");
}

void local_timer_handler(){
	printf("Local timer interrupt, jiffies %d \r\n",local_timer_count);
	local_timer_count += 1;
	*LOCAL_TIMER_IRQ_CLR = (0xc0000000);// clear interrupt and reload.
	return;
}

void core_timer_handler() 
{
	printf("Arm core timer interrupt, jiffies %d \r\n",core_timer_count);
	unsigned int timer_freq;
	asm volatile("mrs %0, sp_el0": "=r"(timer_freq)::);
	printf("now stack pointer is %x \r\n",timer_freq);

	core_timer_count += 1;
	current->counter--;
	if(current->counter <= 0){
		current->rescheduled = 1;
	}
	clean_core_timer();
	return;
}

void handle_irq(void)
{
	unsigned int arm_local = *CORE0_INTR_SRC;
  	if (arm_local & 0x800){
		local_timer_handler();
    }
  	else if (arm_local & 0x2){
    	core_timer_handler();
    }
	else{
		printf("%%%%%%%%%%%%%%%%%%%%%%\n");
	}
	check_reschedule();
}

