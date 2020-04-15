#include "entry.h"
#include "uart.h"
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

#define LOCAL_TIMER_CONTROL_REG 0x40000034

void local_timer_init(){
  unsigned int flag = 0x30000000; // enable timer and interrupt.
  unsigned int reload = 25000000;
  put32(LOCAL_TIMER_CONTROL_REG, flag | reload);
}

#define LOCAL_TIMER_IRQ_CLR 0x40000038

void local_timer_handler(){
  put32(LOCAL_TIMER_IRQ_CLR, 0xc0000000); // clear interrupt and reload.
}


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

void sync_handler(unsigned long x0){
    unsigned long current_el;
    current_el = get_el();
    uart_puts("Current EL: ");
    uart_hex(current_el);
    uart_puts("\n");

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
    //esr[31:26]
    uart_puts("EC: 0x");
    uart_hex((unsigned int)(esr>>26));
    
    //esr[24:0]
    uart_puts(", ISS: 0x");
	uart_hex((unsigned int)(iss));

    uart_puts(", return address: 0x");
    uart_hex((unsigned int)(elr));
    uart_puts("\n");
   	
	if (iss == 0x80)
    {

        uart_puts("System Call[");
        uart_hex(x0);
        uart_puts("]\n");
        switch (x0)
        {
        // arm core timer
        case 0x0:
            enable_irq();
            core_timer_enable();
            break;
        // arm local timer
        case 0x1:
            enable_irq();
			local_timer_init();
            break;
        // not this syscall
        default:
            uart_puts("Cannot find this system call");

            while (1)
                ;
        }
    }  

}

extern int core_jiffies = 0;
extern int local_jiffies = 0;
#define CORE0_IRQ_SOURCE (volatile unsigned int *)0x40000060
void irq_handler(void)
{
/*	unsigned int CORE0 = *CORE0_IRQ_SOURCE;
    if((CORE0 & 0x400) == 0x400){
		local_timer_handler();
		local_jiffies += 1;
		uart_puts("Local timer interrupt, jiffies ");
		uart_hex(local_jiffies);
		uart_puts("\n");
	} else {
*/    
		core_timer_handler();
		core_jiffies += 1;
		uart_puts("Core timer interrupt, jiffies ");
		uart_hex(core_jiffies);
		uart_puts("\n");
//	}
}


