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

    // relocate our code from load address to link address
/*
    asm volatile(
        "ldr x1, =0x80000;"
        "ldr x2, =_start;"
    );

    asm volatile(
        "ldr w3, %0;" : "=r"(kernel_size)
    );

    asm volatile(
        "_l_load_images:"
        "ldr x4, [x1], #8;"
        "str x4, [x2], #8;"
        "sub w3, w3, #1;"
        "cbnz w3, _l_load_images;"
    );
*/


    // restore arguments and jump to the new kernel.
    
   // asm volatile("mov x30, %0;" : "=r"((unsigned long int *)load_address));
   //  asm volatile("br %0" : "=r"(load_address));
    
/*
    asm volatile(
        "ldr x1, =_start;"
        "mov sp, x1;"
    );
*/
/*
    asm volatile (
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
    );
*/
    asm volatile("mov x30, 0x80000; ret;");

    /*
    // asm volatile("b #0x80000");
    // asm volatile("bl main");
    // asm volatile("bl pcsh");
    */
}

void copy_kernel_and_load_images(void *load_address, int kernel_size)
{
    char *start = (char *)0x80000;
    char *end = __bss_end;
    // end = load_address + kernel_size;
    int address_bias =  - 0x10000;
    char *copy = (char *)(start + address_bias);
    

/*
    asm volatile (
        "mov x10, x0;"
        "mov x11, x1;"
        "mov x12, x2;"
        "mov x13, x3;"
    );
*/
    
    uart_puts("Start Copy Kernel\n");
    while (start <= end)
    {
        *copy = *start;
        start++;
        copy++;
    }
    uart_puts("Copy Kernel DONE\n");

    void (*func_ptr)(void*, int) = load_images;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(void*, int) = (void (*)(void*, int))(original_function_address + address_bias);
    call_function(load_address, kernel_size);
}
#endif
