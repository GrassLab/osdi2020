#include "uart.h"

void shell_init() {
    uart_init();
}

void shell_input(char* cmd) {
    uart_printf("\r# ");

    int idx = 0;
    char c;
    while ((c = uart_read()) != '\n') {
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
        uart_printf("\r# %s", cmd);
    }

    uart_printf("\n");
}

void shell_controller(char* cmd) {
    uart_printf("%s\n", cmd);
}