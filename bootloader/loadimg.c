#include "uart0.h"

int loadimg() {
    uart_printf("Send image via UART now!\n");

    // big endian
    int img_size = 0, i;
    for (i = 0; i < 4; i++) {
        img_size <<= 8;
        img_size |= (int) uart_read();
    }

    int img_checksum = 0;
    for (i = 0; i < 4; i++) {
        img_checksum <<= 8;
        img_checksum |= (int) uart_read();
    }

    uart_printf("%d %d\n", img_size, img_checksum);

    return 0;
}