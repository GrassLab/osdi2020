#include "tools.h"

char *entry_error_messages[] = {
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
	"ERROR_INVALID_EL0_32",

	"SYNC_ERROR",
	"SYSCALL_ERROR"
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
  uart_puts (entry_error_messages[type]);
  uart_puts ("\n ESR: ");
  uart_hex(esr);
  uart_puts ("\n address: ");
  uart_hex(address);
  uart_puts ("\n ");
}

int core_counter=0, local_counter=0;

int is_local_timer ()
{
  return *LOCAL_TIMER_CONTROL_REG & 0x80000000; //31 Interrupt flag (Read-Only)
}

int is_core_timer ()
{
  int cntp_ctl_el0;
  asm volatile ("mrs %0, cntp_ctl_el0":"=r" (cntp_ctl_el0)); 
  return cntp_ctl_el0 & 4; //ISTATUS, bit [2] if Timer condition is met return 1
}	
void irq_router ()
{
    if (is_core_timer ())
    {
        uart_puts ("core timer: ");
        uart_send_int (++core_counter);
        uart_send ('\n');
        core_timer_handler ();
        update_task_counter();
    }
    else if (is_local_timer ())
    {
        uart_puts ("local timer: ");
        uart_send_int (++local_counter);
        uart_send ('\n');
        local_timer_handler ();
    }
    else
    {
        uart_puts ("Unknown IRQ\n");
    }
}
