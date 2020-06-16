#include "uart.h"
extern unsigned long _end;
void main()
{
    // set up serial console
    uart_init();
    
    // test our printf implementation
    printf("Hello %s!\n", "MFK");
    printf("hex: 0x%x\n", &_end);

    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}
