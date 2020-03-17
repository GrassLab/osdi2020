#include "uart.h"
#include "shell.h"

int main(){
    char buf[256];
    int i = 0;

    uart_init();
    uart_puts("-----------------------------------------\n");
    uart_puts("|                welcome!!!             |\n");
    uart_puts("-----------------------------------------\n");
    uart_puts("# ");
    while(1){
        buf[i] = uart_getc();
        uart_send(buf[i]);
        if (buf[i] == '\n' || buf[i] == '\r'){
            i = 0;

            run(buf);
            uart_puts("# ");
            continue;
        }

        i++;
    }
}
