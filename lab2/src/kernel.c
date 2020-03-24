#include "uart.h"
#include "utils.h"
#include "mbox.h"
extern char bss_end[];

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0') {
            return 0;
        }

        str1++;
        str2++;
    }
}

void copy_and_jump_to_kernel() {
    int kernel_size = uart_read_int();
    uart_send_int(kernel_size);

    char *kernel = (char *)0;
    int checksum = 0;
    for (int i = 0; i < kernel_size; i++) {
        char c = uart_recv();
        checksum += c;
        kernel[i] = c;
    }
    uart_send_int(checksum);

    uart_send_string("Done copying kernel\r\n");
    branch_to_address((void *)0x00);
}


void copy_current_kernel_and_jump(char *new_address) {
    char *kernel = (char *)0x80000;
    char *end = bss_end;
    int checksum = 0;
    int kernel_size;
    char *copy = new_address;

    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }

    kernel_size = uart_read_int();
    uart_send_int(kernel_size);

    for (int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_recv();
        checksum += (int)c;
        // kernel[i] = c;
    }
    
    uart_send_int(checksum);

    // uart_send_string("Done copying kernel\r\n");
    // branch_to_address((void *)0x80000);
}


void kernel_main(void) {
    int buff_size = 100;
    uart_init();
    char buffer[buff_size];
    while (1) {
        readline(buffer, buff_size);
        if (strcmp(buffer, "hello") == 0) {
            uart_send_string("Hello!!!\n");
        } 
        if (strcmp(buffer, "revision") == 0) {
            get_board_revision();
        }
        if (strcmp(buffer, "address") == 0) {
            get_vc_base_address();
        }
        if (strcmp(buffer, "initframe") == 0) {
            write_buf(framebuffer_init());
        }
        if (strcmp(buffer, "kernel") == 0) {
            uart_send_string("kernel\n");
            copy_current_kernel_and_jump((char *)0x80000);
        }
    } 
}