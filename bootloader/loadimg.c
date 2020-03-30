#include "uart0.h"

int loadimg() {
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

    uart_printf("Image Size: %d, Checksum: %d\n", img_size, img_checksum);

    for (i = 0; i < img_size; i++) {
        char b = uart_read_raw();
        img_checksum -= (int)b;
    }

    if (img_checksum != 0) {
        uart_printf("Failed!");
    }

    return 0;
}