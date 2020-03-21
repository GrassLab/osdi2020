#include "include/uart.h"
#include "include/power.h"
#include "include/time.h"

void main()
{
    // set up serial console
    uart_init();

    loop_start:do {
	
        char tmp;
        char user_input[12];
        int i = 0;
        uart_send('>');
        while (i < 10) {
            tmp = uart_getc();
            if (tmp == '\n') break;
            uart_send(tmp);
            user_input[i++] = tmp;
        }
        user_input[i] = '\0';
        uart_send('\n');
        //uart_send(uart_i2c(i));
        if (i == 0) {
            goto loop_start;
        }

        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("Hello World!\n");
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n");
            reset(10);
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            goto loop_start;    
        }
        if (uart_strcmp(user_input, "help") == 0) {
            uart_puts("hello: print hello world.\n");
            uart_puts("help: help.\n");
            uart_puts("reboot: restart.\n");
            uart_puts("time: show timestamp.\n");
            goto loop_start;
        }
        uart_puts("Error: command ");
        uart_puts(user_input);
        uart_puts(" not found, try <help>.\n");
    } while (1);
}
