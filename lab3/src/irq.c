#include "utils.h"
#include "timer.h"
#include "entry.h"
#include "irq.h"
#include "config.h"

unsigned int c = 0;
unsigned int local_timer_count = 0;
unsigned int core_timer_count = 0;


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
	// printf("%d, %s, ESR: %x, address: %x\r\n", type ,entry_error_messages[type], esr, address);
}

void handle_el0_sync(unsigned long esr, unsigned long address)
{
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


void local_timer_handler(){
	_print("Local timer interrupt, jiffies ");
	uart_send_int(local_timer_count);
	_print("\n");
	local_timer_count += 1;
	*LOCAL_TIMER_IRQ_CLR = (0xc0000000);// clear interrupt and reload.
	return;
}

void core_timer_handler() 
{
	// _print("Arm core timer interrupt, jiffies ");
	// uart_send_int(core_timer_count);
	// _print("\n");
	// core_timer_count = core_timer_count + 1;
	clean_core_timer();
	return;
}

void handle_irq(void)
{
	unsigned int arm_local = *CORE0_INTR_SRC;
  	if (arm_local & 0x800)
    {
      // local timer interrupt
		local_timer_handler();
    }
  	else if (arm_local & 0x2)
    {
		_print("Arm core timer interrupt, jiffies ");
		uart_send_int(core_timer_count);
		_print("\n");
		core_timer_count = core_timer_count + 1;
		// _print("Arm core timer interrupt, jiffies ");
		//  uart_send_int(c);
		// _print("\n");
		// c += 1;

      	// core timer interrupt
    	core_timer_handler();
    }

}
