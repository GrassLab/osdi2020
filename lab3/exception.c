#include "uart.h"

void
_el2_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
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

void
_el1_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
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

void
_context_switch_msg()
{
    uart_puts("context switching...\r\n");
}

void
_not_implement()
{
    uart_puts("Not implement.\r\n");
}
