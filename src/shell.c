#include "uart.h"
#include "my_string.h"
#include "utli.h"

void shell_init() {
    uart_init();
}

void shell_input(char* cmd) {
    uart_printf("\r# ");

    int idx = 0;
    cmd[0] = '\0';
    char c;
    while ((c = uart_read()) != '\n') {
        // Backspace
        if (c == 8 || c == 127) {
            if (idx > 0){
                idx--;
                cmd[idx] = '\0';
            }
        }
        else {
            cmd[idx++] = c;
            cmd[idx] = '\0';
        }
        uart_printf("\r# %s \r\e[%dC", cmd, idx + 2);
    }

    uart_printf("\n");
}

void shell_controller(char* cmd) {
    if (!strcmp(cmd, "help")) {
        uart_printf("help: print all available commands\n");
        uart_printf("hello: print Hello World!\n");
        uart_printf("timestamp: get current timestamp\n");
    }
    else if (!strcmp(cmd, "hello")) {
        uart_printf("Hello World!\n");
    }
    else if (!strcmp(cmd, "timestamp")) {
        uart_printf("%f\n", get_timestamp()); 
    }
    else {
        uart_printf("shell: command not found: %s\n", cmd);
    }
}