#include "uart.h"

int main(){

    // set uart
    uart_init();
    
    uart_puts("0856168 Hsu, Po-Chun\n");
    uart_puts("Hello World!\n");
    

    // echo
    while(1) {
        uart_send(uart_getc());
    }

}
