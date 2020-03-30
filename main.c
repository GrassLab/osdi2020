#include "include/uart.h"
#include "include/power.h"
#include "include/time.h"
#include "include/mailbox.h"
#include "include/info.h"

void main()
{
    // set up serial console
    uart_init();

    get_serial();
    get_board_revision();
    get_vccore_addr();

    while (1) {
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
        uart_puts("\n");
        //uart_send(uart_i2c(i));
        if (i == 0) {
            continue;
        }
        /*
         * <hello> Echo hello
         */
        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("Hello World!\n");
            continue;
        }
        /*
         * <reboot> reboot in given cpu ticks
         */
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n\n");
            reset(10);
            continue;
        }
        /*
         * <time> get the timestamp
         */
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            continue;    
        }
        /*
         * <loadimg> listen for raspbootcom and load the img from UART
         */
        if (uart_strcmp(user_input, "loadimg") == 0) {
            uart_puts("loading image from uart...\n");
            continue;    
        }
        /*
         * <help> list the existed commands
         */
        if (uart_strcmp(user_input, "help") == 0) {
            uart_puts("hello: print hello world.\n");
            uart_puts("help: help.\n");
            uart_puts("reboot: restart.\n");
            uart_puts("time: show timestamp.\n");
            uart_puts("loadimg: reload image from UART.\n");
            continue;
        }
        /*
         * Invalid command
         */
        uart_puts("Error: command ");
        uart_puts(user_input);
        uart_puts(" not found, try <help>.\n");
    }
}
