#include "uart.h"
#include "utils.h"
#include "mbox.h"
#include "uart.h"
#define  TMP_KERNEL_ADDR  0x100000
extern char bss_end[];
extern char start_begin[];

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

void copy_and_jump_to_kernel(char *new_address) {
    char *kernel = new_address;
    int kernel_size = uart_read_int();
    uart_send_int(kernel_size);

    int checksum = 0;
    for (int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_recv();
        checksum += c;
        kernel[i] = c;
    }
    uart_send_int(checksum);
    branch_to_address((unsigned long int *)new_address);
}

void copy_current_kernel_and_jump(char *new_address) {
    char *kernel = start_begin;
    char *end = bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
    uart_send_string("begin of copy kernel\n");
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_send_string("end of copy kernel\n");
    void (*func_ptr)() = copy_and_jump_to_kernel;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(char *) = (void (*)(char *))(original_function_address - (unsigned long int)start_begin + TMP_KERNEL_ADDR);
    call_function(new_address);
}


void kernel_main(void) {
    int buff_size = 100;
    char buffer[buff_size];
    uart_init();
    while (1) {
        readline(buffer, buff_size);
        if (strcmp(buffer, "hello") == 0) {
            uart_send_string("Hello!!!\n");
        } 
        if (strcmp(buffer, "revision") == 0) {
            uart_send_string("revision\n");   /* send board revision and VC Core base address.  */
            get_board_revision();
        }
        if (strcmp(buffer, "address") == 0) {
            uart_send_string("address\n");  /* send board revision and VC Core base address.  */
            get_vc_base_address();
        }
        if (strcmp(buffer, "initframe") == 0) {
            uart_send_string("initframe\n");
            write_buf(framebuffer_init());
        }
        if (strcmp(buffer, "kernel") == 0) {
            unsigned long long get_kernel_address;
            uart_send_string("kernel\n");   
            get_kernel_address = read_kernel_address();
            copy_current_kernel_and_jump((char *)get_kernel_address);
        }
    } 
}