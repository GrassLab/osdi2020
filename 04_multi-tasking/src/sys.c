#include "peripherals/uart.h"

/*
void handle_sync(unsigned long esr, unsigned long address) {
    uart_send_hex(address);
    uart_send_hex(esr >> 26);
    uart_send_hex(esr & 0x1ffffff);
}
*/

void handle_el0_sync(unsigned long arg1, unsigned long arg2) {
    // uart_send('b');
}

void handle_el1_sync(unsigned long esr, unsigned long address) {
    // uart_send('c');
}
