#include "uart.h"
#include "common.h"

extern char __bss_end[];

void load_images(int kernel_size)
{
    char *address = (char *)0x80000;

    for (int i = 0; i < kernel_size; i++)
    {
        unsigned char b = uart_getc();
        // check no loss package
        uart_send_int(i);
        address[i] = b;
    }
    uart_puts("Done copying kernel\n");
    // asm volatile("b #0x80000");
    char *start = (char *)0x80000;
    asm volatile("br %0"
                 : "=r"(start));
    //asm volatile("bl main");
}

void copy_kernel_and_load_images(int kernel_size)
{
    char *start = (char *)0x80000;
    char *end = __bss_end;
    char *copy = (char *)0x88000;

    while (start <= end)
    {
        *copy = *start;
        start++;
        copy++;
    }

    void (*func_ptr)(int) = load_images;
    long long int original_function_address = (long long int)func_ptr;
    void (*call_function)(int) = (void (*)(int))(original_function_address + 0x8000);
    uart_puts("Start to load image by uart\n");
    call_function(kernel_size);
}
