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