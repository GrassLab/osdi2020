#include "peripherals/uart.h"
#include "sys.h"

/*
void handle_sync(unsigned long esr, unsigned long address) {
    uart_send_hex(address);
    uart_send_hex(esr >> 26);
    uart_send_hex(esr & 0x1ffffff);
}
*/

int handle_el0_sync(unsigned long arg0, unsigned long arg1) {
    
    int syscall;
    asm("mov %0, x8" : "=r"(syscall));
    /* 
    uart_puts("Receive syscall\n");
    uart_send_ulong(syscall);
    uart_send('\n');
    uart_send_ulong(arg0);
    uart_send('\n');
    uart_send_ulong(arg1);
    uart_send('\n');
    */
    if (syscall == SYS_UART_WRITE) {
        for (int i = 0; i < arg1; ++ i) {
            uart_send(((char*)arg0)[i]);
        }
        return arg1;
    } else if (syscall == SYS_UART_READ) {
        for (int i = 0; i < arg1; ++ i) {
            ((char*)arg0)[i] = uart_getc();
        }
        return arg1;
    } else if (syscall == SYS_EXEC) {
        return do_exec(arg0);
    } else if (syscall == SYS_FORK) {
        return _fork();
    } else {
        uart_puts("unknown syscall\n");
    }
}

void handle_el1_sync(unsigned long esr, unsigned long address) {
    // uart_send('c');
}
