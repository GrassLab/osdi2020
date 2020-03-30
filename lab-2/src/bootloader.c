#include "bootloader.h"
#include "uart.h"

extern char __bss_end[];

void load_new_kernel(char *new_address, int size) {
    char *kernel = new_address;
    for (int i = 0; i < size; i++) {
        unsigned char c = uart_getc();
        kernel[i] = c;
    }
    uart_puts("Finished copy new kernel and jump.\n");
    void (*target)() = new_address;
    target();
}

void load_image(char *new_address, int size) {
    char *kernel = 0x80000;
    char *end = __bss_end;
    char *copy = (char *)(0x100000);
    uart_puts("begin of copy kernel\n");
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("end of copy kernel\n");
    void (*func_ptr)() = load_new_kernel;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(char *, int) = (void (*)(char *))(original_function_address - (unsigned long int)0x80000 + 0x100000);
    call_function(new_address, size);
}