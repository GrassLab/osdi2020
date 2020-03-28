#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "uart.h"
#include "common.h"

extern char __bss_end[];
extern char __end[];

void load_images(void *load_address, int kernel_size)
{
    char *address = (char *)load_address;

    uart_puts("Start to load image by uart\n");
    for (int i = 0; i < kernel_size; i++)
    {
        unsigned char b = uart_getc();
        // check no loss package
        uart_send_int(i+1);
        address[i] = b;
    }
    uart_puts("Load kernel image by uart DONE\n");
    uart_puts("\n");
    // asm volatile("b #0x80000");
    // asm volatile("bl main");
    // asm volatile("bl pcsh");
    asm volatile("br %0"
                 : "=r"(load_address));
}

void copy_kernel_and_load_images(void *load_address, int kernel_size)
{
    char *start = (char *)0x0000;
    char *end = __bss_end;
    // end = load_address + kernel_size;
    int address_bias = ((char *)load_address - start) + 0x8000;
    char *copy = (char *)(start + address_bias);
    
    uart_puts("Start Copy Kernel\n");
    while (start <= end)
    {
        *copy = *start;
        start++;
        copy++;
    }

    void (*func_ptr)(void*, int) = load_images;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(void*, int) = (void (*)(void*, int))(original_function_address + address_bias);
    uart_puts("Copy Kernel DONE\n");
    call_function(load_address, kernel_size);
}
#endif
