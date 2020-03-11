#include "uart.h"
#include "pcsh.h"

int main(){

    // set uart
    uart_init();
    
    uart_puts("0856168 Hsu, Po-Chun\n");
    uart_puts("Hello World!\n");

    pcsh();

    // echo
    while(1) {
        uart_send(uart_getc());
    }

}
