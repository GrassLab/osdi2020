#include "uart.h"

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
    uart_printf("%s\n", cmd);
}