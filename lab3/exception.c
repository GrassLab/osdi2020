#include "uart.h"

void
el2_not_implement()
{
    uart_puts("Not implement.\r\n");
}

void
el2_current_sp2_sync_message(unsigned long elr_el2, unsigned long esr_el2)
{
    // todo
    uart_puts("elr_el2: ");
    uart_hex(elr_el2);
    uart_puts("\n");
    uart_puts("esr_el2: ");
    uart_hex(esr_el2);
    uart_puts("\n");
}

void
_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    uart_puts("Exception Class: 0x");
    uart_hex(esr>>(32-6));
    uart_puts("\r\nInstruction Specific syndrome: 0x");
    uart_hex(esr&&(0x100));
    uart_puts("\r\nException return address: 0x");
    uart_hex(elr>>32);
    uart_hex(elr);
    uart_puts("\r\n");
}
