#include "uart.h"
//#include "power.h"
#include "time.h"

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
        //uart_puts(user_input);
        if (i == 0) {
            uart_puts("wrong input\n");
            goto loop_start;
        }

        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("hello world\n");
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            reset(3);
            get_time();
            goto loop_start;
        }
        
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            goto loop_start;    
        }
        if (uart_strcmp(user_input, "help") == 0) {
            uart_puts("help me\n");
            goto loop_start;
        }
        uart_puts("invalid input\n");
    } while (1);
}
