#include "uart.h"

int main(){
    uart_init();
   
    char buf[256];
    int i = 0; 
    while(1){
        buf[i] = uart_getc();
        uart_send(buf[i]);
        if (buf[i] == '\n' || buf[i] == '\r'){
            uart_puts(buf);
            i = 0;
            continue;
        }
        i++;
    }
}
