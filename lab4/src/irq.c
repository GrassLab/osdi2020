#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals.h"
#include "tools.h"
#include "uart.h"
#include "irq.h"
#include "multitask.h"

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

// include/irq.h
void enable_timer1_interrupt_controller(){

	// set bit 1(system timer match 1)
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

// src/entry.S : handle_invalid_entry_type
void show_invalid_entry_message(unsigned long type, unsigned long esr, unsigned long address){
	
	uart_puts("which type: ");
	uart_int(type);
	uart_puts("\n");


	char buf_address[100];
	char buf_ec[100];
	char buf_iss[100];

	itoa(address, buf_address, 10);
    uart_puts("Exception return address: 0x");
    //uart_puts(buf_address);
    uart_hex(address);
    uart_puts("\n");

	itoa((esr & 0xFC000000) >> 26, buf_ec, 10);
	uart_puts("Exception class (EC): 0x");
    //uart_puts(buf_ec);
	uart_hex((esr & 0xFC000000) >> 26);
    uart_puts("\n");

	itoa((esr & 0x01FFFFFF), buf_iss, 10);
	uart_puts("Instruction specific syndrome (ISS): 0x");
    //uart_puts(buf_iss);
	uart_hex((esr & 0x01FFFFFF));
    uart_puts("\n");
}


// src/entry.S : el1_irq
void handle_irq(void){

	unsigned int irq = get32(IRQ_PENDING_1);
	switch (irq) {
		case (SYSTEM_TIMER_IRQ_1):
			uart_puts("System timer 1\n");
			break;
		default:
			uart_puts("Unknown pending irq: XXX\n");
			//printf("Unknown pending irq: %x\r\n", irq);
	}
}

void system_call(unsigned long syscall_number, unsigned long x0, unsigned long x1){

	if(syscall_number == 1){	// core timer enable
		core_timer_enable();
	}else if(syscall_number == 2){	// get_taskid
		int pid = get_current();
		asm volatile("mov x0, %0" ::"r"(pid));
	}else if(syscall_number == 3){	// exec
		do_exec(x1);
	}else if(syscall_number == 4){	// fork
		unsigned long * x0_ptr = (unsigned long *)x0;
		*x0_ptr = do_fork();
	}else if(syscall_number == 5){	// exit
		exit_process(get_current());
	}else if(syscall_number == 6){	// sys_uart_read
		
	}else if(syscall_number == 7){	// sys_uart_write
		
	}else{
		uart_puts("no such system call number!\n");
	}
	return;
}

// src/entry.S	vector entry 9th
void sync_handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4, unsigned long x5){

	unsigned long elr_el1;
	unsigned long esr_el1;
	unsigned long syscall_number;
	unsigned long _x0, _x1;

	// read the system call number
	asm volatile("mrs %0, elr_el1\n" : "=r"(elr_el1));	// exception return address
	asm volatile("mrs %0, esr_el1\n" : "=r"(esr_el1));	// read the exception class
	asm volatile("mov %0, x8\n" : "=r"(syscall_number));	// system call number is saved in x8 register
	asm volatile("mov %0, x0\n" : "=r"(_x0));
	asm volatile("mov %0, x1\n" : "=r"(_x1));

	if(( (esr_el1 & 0xFC000000) >> 26) == 0x15){	// SVC instruction execution in AArch64 state
		
		// svc #0: 0x56000000
		// svc #1: 0x56000001
		if((esr_el1 == 0x56000000)){ // svc #0
			system_call(syscall_number, _x0, _x1);
			return;
		}else{	// svc #1
			show_invalid_entry_message(-1, esr_el1, elr_el1);
		}
	}else{
		// pass
	}
}

 
// src/entry.S	vector entry 10th
void irq_handler(){

	//uart_puts("irq handler\n");
	unsigned int IRQ_SOURCE = *((volatile unsigned int*)CORE0_IRQ_SOURCE);

	// bit 1: CNTPNSIRQ interrupt
	if(IRQ_SOURCE == 0x00000002){	// core timer IRQ_SOURCE: 2
		
		//uart_puts("Core timer interrupt, jiffies\n");
		core_timer_handler();

		return;	
	}

}
