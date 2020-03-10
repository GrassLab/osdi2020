#include "uart.h"

int main()
{
    char buffer[10] = {'\0'};
    unsigned int count = 0;
    // set up serial console
    uart_init();
    
    // echo everything back
    while(1) {
        char c = uart_getc();
        uart_send(c);
    }
}
