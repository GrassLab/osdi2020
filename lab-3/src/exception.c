void exception_handler()
{
    unsigned int esr, elr, spsr, far;
    asm volatile ("mrs %0, esr_el1" : "=r" (esr));
    asm volatile ("mrs %0, elr_el1" : "=r" (elr));
    asm volatile ("mrs %0, spsr_el1" : "=r" (spsr));
    asm volatile ("mrs %0, far_el1" : "=r" (far));
    uart_puts("[info] Exception Class: 0x");
    uart_print_hex(esr>>(32-6));
    uart_puts("\n[info] Instruction Specific syndrome: 0x");
    uart_print_hex(esr&&(0x100));
    uart_puts("\n[info] Exception return address: 0x");
    uart_print_hex(elr>>32);
    uart_print_hex(elr);
    uart_puts("\n");
}