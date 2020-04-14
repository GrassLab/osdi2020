#include "tools.h"

void exception_handler (void)
{
    int address, syndrome;
    asm volatile ("mrs %0, elr_el1\n"
            "mrs %1, esr_el1\n":"=r" (address), "=r" (syndrome));
    int iss = syndrome & 0x01ffffff;
    int ec = (syndrome & 0xfc000000) >> 26;
//   printf ("Exception return address 0x%p\r\n", (void *) elr_el2);
//   printf ("Exception class (EC) 0x%x\r\n", esr_el2 >> 26);
//   printf ("Instruction specific syndrome (ISS) 0x%x\r\n", esr_el2 & 0xffffff);
    if(ec==0x15){
        if(iss==0x1){
            uart_puts("Exception return address: ");
            uart_hex(address);
            uart_send('\n');
            uart_puts("Exception class (EC): ");
            uart_hex(ec);
            uart_send('\n');    
            uart_puts("Instruction specific syndrome (ISS): ");
            uart_hex(iss);
            uart_send('\n');
        }else if(iss==0x2){
            uart_puts("enable time\n");
            core_timer_enable();
            local_timer_init ();
        }
    }
}
void not_implemented ()
{
  uart_puts ("kernel panic - function not implemented!\n");
  while (1);
}