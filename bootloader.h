#include "uart.h"
#include "common.h"

extern char __bss_end[];

void load_images(void *load_address, int kernel_size)
{
    char *address = (char *)load_address;
    int checksum = 0;

    for (int i = 0; i < kernel_size; i++)
    {
        unsigned char b = uart_getc();
        checksum += b;
        // check no loss package
        uart_send_int(i);
        address[i] = b;
    }
    uart_puts("Done copying kernel\n");
    uart_send_int(checksum);
    uart_puts("\n");
    // asm volatile("b #0x80000");
    char *start = (char *)load_address;
    asm volatile("br %0"
                 : "=r"(start));
    //asm volatile("bl main");
}

void copy_kernel_and_load_images(void *load_address, int kernel_size)
{
    char *start = (char *)load_address;
    char *end = __bss_end;
    char *copy = (char *)0x88000;

    while (start <= end)
    {
        *copy = *start;
        start++;
        copy++;
    }

    void (*func_ptr)(void*, int) = load_images;
    long long int original_function_address = (long long int)func_ptr;
    void (*call_function)(void*, int) = (void (*)(void*, int))(original_function_address + 0x8000);
    uart_puts("Start to load image by uart\n");
    call_function(load_address, kernel_size);
}
