void exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    uart_puts("[info] Exception Class: 0x");
    uart_print_hex(esr>>(32-6));
    uart_puts("\n[info] Instruction Specific syndrome: 0x");
    uart_print_hex(esr&&(0x100));
    uart_puts("\n[info] Exception return address: 0x");
    uart_print_hex(elr>>32);
    uart_print_hex(elr);
    uart_puts("\n");
}