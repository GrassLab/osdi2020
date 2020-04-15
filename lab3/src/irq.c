#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals.h"
#include "tools.h"
#include "uart.h"
#include "irq.h"

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
void show_invalid_entry_message(int type, unsigned long esr, unsigned long address){

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

/*
	if(type == -1){
		uart_puts("entry error message: ");	uart_puts(entry_error_messages[type]);
		uart_puts("\n");	uart_puts("ESR: ");
		itoa(esr, buf_esr, 10);	uart_puts(buf_esr);	uart_puts("\n");
		itoa(address, buf_address, 10);
		uart_puts("address: ");	uart_puts(buf_address); uart_puts("\n");
	}	
*/

}

void print_system_registers(){
	unsigned int elr_el1, elr_el2, elr_el3;
	unsigned int esr_el1, esr_el2, esr_el3;
	unsigned int currentel, daif, nzcv, spsel;	

	char buf[100];

	asm volatile("mrs %0, elr_el1\n" : "=r"(elr_el1));
	itoa(elr_el1, buf, 10);
	uart_puts("ELR_EL1: 0x");
	//uart_puts(buf);
	uart_hex(elr_el1);
	uart_puts("\n");
/*
	asm volatile("mrs %0, elr_el2\n" : "=r"(elr_el2));
	itoa(elr_el2, buf, 10);
	uart_puts("ELR_EL2: ");
	uart_puts(buf);
	uart_puts("\n");
	
	asm volatile("mrs %0, elr_el3\n" : "=r"(elr_el3));
	itoa(elr_el3, buf, 10);
	uart_puts("ELR_EL3: ");
	uart_puts(buf);
	uart_puts("\n");
*/
	asm volatile("mrs %0, esr_el1\n" : "=r"(esr_el1));
	itoa(esr_el1, buf, 10);
	uart_puts("ESR_EL1: 0x");
	//uart_puts(buf);
	uart_hex(esr_el1);
	uart_puts("\n");
/*
	asm volatile("mrs %0, esr_el2\n" : "=r"(esr_el2));
	itoa(esr_el2, buf, 10);
	uart_puts("ESR_EL2: ");
	uart_puts(buf);
	uart_puts("\n");
	
	asm volatile("mrs %0, esr_el3\n" : "=r"(esr_el3));
	itoa(esr_el3, buf, 10);
	uart_puts("ESR_EL3: ");
	uart_puts(buf);
	uart_puts("\n");
*/
	asm volatile("mrs %0, CurrentEL\n" : "=r"(currentel));
	currentel = currentel >> 2;
	itoa(currentel, buf, 10);
	uart_puts("CURRENT EXCEPTION LEVEL: ");
	uart_puts(buf);
	uart_puts("\n");

	asm volatile("mrs %0, DAIF\n" : "=r"(daif));
	itoa(daif, buf, 10);
	uart_puts("DAIF: ");
	uart_puts(buf);
	uart_puts("\n");

	asm volatile("mrs %0, NZCV\n" : "=r"(nzcv));
	itoa(nzcv, buf, 10);
	uart_puts("NZCV: ");
	uart_puts(buf);
	uart_puts("\n");

	asm volatile("mrs %0, SPSel\n" : "=r"(spsel));
	itoa(spsel, buf, 10);
	uart_puts("SPSEL: ");
	uart_puts(buf);
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

// src/entry.S	vector
void test_handler_sync_invalid_el0_64(void){
	uart_puts("sync invalid el0 64\n");
}

// src/entry.S	vector
void test_handler_irq_invalid_el0_64(void){
	uart_puts("irq invalid el0 64\n");
}

void system_call(unsigned int syscall_number){

	if(syscall_number == 1){	// core timer enable
		core_timer_enable();
	}else if(syscall_number == 2){
		uart_puts("system call 2 test\n");	
	}else if(syscall_number == 3){
		print_system_registers();
	}else{
		uart_puts("no such system call number!\n");
	}
	return;
}

// src/entry.S	vector entry 9th
void sync_el0_64_handler(int x0, int x1, int x2, int x3, int x4, int x5){

	unsigned int elr_el1;
	unsigned int esr_el1;
	unsigned int syscall_number;

	// read the system call number
	asm volatile("mrs %0, elr_el1\n" : "=r"(elr_el1));	// exception return address
	asm volatile("mrs %0, esr_el1\n" : "=r"(esr_el1));	// read the exception class
	asm volatile("mov %0, x8\n" : "=r"(syscall_number));	// system call number is saved in x8 register

	if(( (esr_el1 & 0xFC000000) >> 26) == 0x15){	// SVC instruction execution in AArch64 state
		
		//uart_puts("esr_el1: ");
		//uart_hex(esr_el1);
		//uart_puts("\n");

		// svc #0: 0x56000000
		// svc #1: 0x56000001
		if((esr_el1 == 0x56000000)){ 
			system_call(syscall_number);
			return;
		}else{
			show_invalid_entry_message(-1, esr_el1, elr_el1);
		}
	}else{
		// pass
	}
}

 
// src/entry.S	vector entry 10th
void irq_el0_64_handler(){

	static unsigned long core_timer_number = 1;
	static unsigned long local_timer_number = 1;

	char buf_core_timer_number[1000];
	char buf_local_timer_number[1000];

	unsigned int IRQ_SOURCE = *((volatile unsigned int*)CORE0_IRQ_SOURCE);

	// bit 1: CNTPNSIRQ interrupt
	if(IRQ_SOURCE == 0x00000002){	// core timer IRQ_SOURCE: 2

		core_timer_handler();

		uart_puts("Core timer interrupt, jiffies ");
		itoa(core_timer_number, buf_core_timer_number, 10);
		uart_puts(buf_core_timer_number);
		uart_puts("\n");

		core_timer_number++;

		return;	
	}

	// bit 11: Local timer interrupt
	if(IRQ_SOURCE == 0x00000800){	// local timer IRQ_SOURCE: 2048
			
		local_timer_handler();
	
		uart_puts("Local timer interrupt, jiffies ");
		itoa(local_timer_number, buf_local_timer_number, 10);
		uart_puts(buf_local_timer_number);
		uart_puts("\n");

		local_timer_number++;

		return;
	}
}
