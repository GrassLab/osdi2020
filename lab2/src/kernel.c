#include "uart.h"
#include "utils.h"
#include "mbox.h"
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

int readline(char *buf, int maxlen) {
    int num = 0;
    while (num < maxlen - 1) {
        char c = uart_recv();
        uart_send(c);
        if (c == '\n' || c == '\0' || c == '\r') {
            break;
        }
        buf[num] = c;
        num++;
    }
    buf[num] = '\0';
    return num;
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
    char *kernel = (char *)0x00;
    char *end = bss_end;

    char *copy = new_address;

    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }

    // Cast the function pointer to char* to deal with bytes.
    char *original_function_address = (char *)&copy_and_jump_to_kernel;

    // Add the new address (we're assuming that the original kernel resides in
    // address 0). copied_function_address should now contain the address of the
    // original function but in the new location.
    char *copied_function_address =
        original_function_address + (long)new_address;

    // Cast the address back to a function and call it.
    void (*call_function)() = (void (*)())copied_function_address;
    call_function();
}


void kernel_main(void) {
    int buff_size = 100;
    uart_init();
    uart_recv();
    char buffer[buff_size];
    while (1) {
        uart_send_string("# ");
        readline(buffer, buff_size);
        if (strcmp(buffer, "hello") == 0) {
            uart_send_string("Hello!!!\r\n");
        } 
        if (strcmp(buffer, "revision") == 0) {
            get_board_revision();
        }
        if (strcmp(buffer, "address") == 0) {
            get_vc_base_address();
        }
        if (strcmp(buffer, "initframe") == 0) {
            framebuffer_init();
            write_buf();
        }
    } 
}