#include "uart.h"

int main() {
    uart_init();

    while (1) {
        uart_write(uart_read());
    }
}