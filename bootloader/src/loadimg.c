#include "uart0.h"

long long address_input() {
    uart_printf("\rPlease input kernel load address (default: 0x80000): 0x");

    int idx = 0, end = 0, i;
    char cmd[128];
    char c;
    while ((c = uart_read()) != '\n') {
        if (end == 8) {
            continue;
        }
        // CTRL-C
        if (c == 3) {
            uart_printf("\n");
            return -1;
        }
        // Backspace
        else if (c == 8 || c == 127) {
            if (idx > 0) {
                idx--;
                // left shift command
                for (i = idx; i < end; i++) {
                    cmd[i] = cmd[i + 1];
                }
                cmd[--end] = '\0';
            }
        }
        else {
            // right shift command
            if (idx < end) {
                for (i = end; i > idx; i--) {
                    cmd[i] = cmd[i - 1];
                }
            }
            cmd[idx++] = c;
            cmd[++end] = '\0';
        }
        uart_printf("\rPlease input kernel load address (default: 0x80000): 0x%s \r\e[%dC", cmd, idx + 55);
    }

    long long address = 0;
    for (i = 0; i < end && cmd[i] != 0; i++) {
        address *= 16;
        address += cmd[i] - '0';
    }

    if (address == 0) {
        uart_printf("\rPlease input kernel load address (default: 0x80000): 0x80000");
        address = 0x80000;
    }

    uart_printf("\n");

    return address;
}

void loadimg() {
    long long address = address_input();

    if (address == -1) {
        return;
    }

    uart_printf("Send image via UART now!\n");

    // big endian
    int img_size = 0, i;
    for (i = 0; i < 4; i++) {
        img_size <<= 8;
        img_size |= (int)uart_read_raw();
    }

    // big endian
    int img_checksum = 0;
    for (i = 0; i < 4; i++) {
        img_checksum <<= 8;
        img_checksum |= (int)uart_read_raw();
    }

    char *kernel = (char *)address;

    for (i = 0; i < img_size; i++) {
        char b = uart_read_raw();
        *(kernel + i) = b;
        img_checksum -= (int)b;
    }

    if (img_checksum != 0) {
        uart_printf("Failed!");
    }
    else {
        void (*start_os)(void) = (void *)kernel;
        start_os();
    }
}