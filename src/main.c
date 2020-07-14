#include "lib/uart.h"
#include "lib/string.h"
#include "lib/reboot.h"

void main() {
    // set up serial console
    uart_init();

    while (1) {
        // print prompt
        uart_send('>');

        // read input
        int i = 0;
        char input[64];
        char c;
        while ((c = uart_getc()) != '\n') {
            input[i++] = c;
        }
        input[i] = '\0';
        uart_send('\n');

        register unsigned long f, t;
        asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
        asm volatile ("mrs %0, cntpct_el0" : "=r"(t));

        if (strcmp(input, "reboot") == 0) {
            // reboot
            reset(3);
            uart_puts("Reboot...\n");
        } else if (strcmp(input, "hello") == 0) {
            // say hello world
            uart_puts("Hello World!\n");
        } else if (strcmp(input, "help") == 0) {
            // print hint
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: help\n");
            uart_puts("reboot: reboot rpi3\n");
            uart_puts("time: get current timestamp\n");
        } else if (strcmp(input, "timestamp") == 0) {
            // print timestamp
            uart_puts("[ ");
            char time_string[16];
            itoa(t, time_string);
            uart_puts(time_string);
            uart_puts(" / ");
            char freq_string[16];
            itoa(f, freq_string);
            uart_puts(freq_string);
            uart_puts(" ]\n");
        } else if (i != 0) {
            // command not found
            uart_puts("Error: command ");
            uart_puts(input);
            uart_puts(" not found, try <help>\n");
        }
    }
}
