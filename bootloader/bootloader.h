#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#define  TMP_KERNEL_ADDR  0x100000
#include "uart.h"
#include "common.h"

extern char __start_begin[];
extern char __bss_end[];
extern char __end[];

void copy_and_jump_to_kernel(char *new_address, int kernel_size) {
    char *kernel = new_address;

    uart_puts("Receive Kernel");
    uart_send_int(kernel_size);

    for (int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_getc();
        kernel[i] = c;
    }
    uart_puts("DONE\n");
    uart_puts("Jump to new kernel\n");
    //branch_to_address((unsigned long int *)new_address);
     asm volatile("br %0" : "=r"((unsigned long int*)new_address));
}

void copy_kernel_and_load_images(char *new_address, int kernel_size) {
    //char *kernel = _start;
    char *kernel = __start_begin;
    char *end = __bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
    uart_puts("copy kernel\n");
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("copy kernel done\n");
    void (*func_ptr)(char *, int) = copy_and_jump_to_kernel;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(char *, int) = (void (*)(char *, int))(original_function_address - (unsigned long int)__start_begin + TMP_KERNEL_ADDR);
    call_function(new_address, kernel_size);
}

#endif
