#include "bootloader.h"
#include "uart.h"

extern char __bss_end[];
extern char __kernel_start[];
unsigned long int backup_address = 0x100000;

void load_target_kernel(char *target_address, int size) {
    uart_puts("[info] Starting to load target kernel\n");
    for (int i = 0; i < size; i++) {
        char c = uart_recv_char();
        target_address[i] = c;
    }
    uart_puts("[info] Finished load target kernel and running.\n");
    void (*target)() = target_address;
    target();
}

void save_running_image()
{
    char *kernel = __kernel_start;
    char *end = __bss_end;
    char *backup = (char *)(backup_address);
    uart_puts("[info] Starting to backup loader kernel\n");
    while (kernel <= end) {
        *backup = *kernel;
        kernel++;
        backup++;
    }
    uart_puts("[info] Finished backup loader kernel\n");
}

void load_image(char *target_address, int size) {
    unsigned long int backup_size = __bss_end - __kernel_start;
    while(backup_address <= target_address + size) {
        backup_address += 0x10000;
    }
    save_running_image();
    void (*load_target_ptr)() = load_target_kernel;
    unsigned long int load_target_func_address = (unsigned long int) load_target_ptr;
    void (*new_load_target_kernel)(char *, int) = 
        (void (*)(char *))(load_target_func_address - (unsigned long int)0x80000 + backup_address);
    new_load_target_kernel(target_address, size);
}