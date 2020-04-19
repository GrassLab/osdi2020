#include "uart.h"
#include "mystd.h"

#define LOADER_START_ADDR 0x80000
#define LOADER_SIZE_MAX 0x10000

extern char __loader_size[];

unsigned int get_kernel_size(){
    unsigned int size;
    size = uart_getc();
    size |= uart_getc()<<8;
    size |= uart_getc()<<16;
    size |= uart_getc()<<24;
    return size;
}

void recieve_kernel(char *load_addr, unsigned int size, unsigned long copy_addr){
    uart_puts("\nloading kernel image...\n\n\n");
    char *kernel = load_addr; 
    // char (*uart_getc_copied)() = copy_addr + (uart_getc - LOADER_START_ADDR);
    // while(size--) 
    //     *kernel++ = uart_getc_copied(); //pointless, the function pointer copy itself whould be overwirte, too 
    while(size--){
        do{asm volatile("nop");} while(*UART0_FR&0x10);
        *kernel++ = (char)(*UART0_DR); 
    }
    asm volatile ("mov x30, %0; ret" ::"r"(load_addr)); 
}

void copy_loader(char *copy_addr){
    //copy loader
    char *loader_old_base = (char*)LOADER_START_ADDR;
    char *loader_new_base = copy_addr;
    unsigned int loader_byte = (unsigned long)__loader_size ;
    while(loader_byte--)
        *loader_new_base++ = *loader_old_base++;

    //copy stack?
    // char *old_stack = (char*)LOADER_START_ADDR-1;
    // char *new_stack = copy_addr-1;
    // char* sp;
    // asm volatile ("mov %0, sp" :"=r"(sp));
    // while(old_stack >= sp)
    //     *new_stack-- = *old_stack--;
    // sp = new_stack + 1;
    // asm volatile ("mov sp, %0" ::"r"(sp));
}

void load_kernel_img(){
    char load_addr[11];
    uart_puts("Please input kernel load address (default: 0x80000)\n");
    uart_gets(load_addr);

    uart_puts("Please send kernel image from UART now...\n");
    unsigned int kernel_size = get_kernel_size();
    uart_puts("Kernel Image Size: ");
    uart_dec(kernel_size);
    uart_puts("    load Addr: ");
    (*load_addr) ? uart_puts(load_addr) : uart_puts("0x80000");
    uart_puts("...\n");
    unsigned long load_addr_ul;
    load_addr_ul = (*load_addr) ? atoi(load_addr) : 0x80000; 

    unsigned long loader_copy_addr;
    loader_copy_addr = load_addr_ul - LOADER_SIZE_MAX;
    copy_loader((char*)loader_copy_addr);

    void (*recieve_kernel_copied)(char*, unsigned int, unsigned long) =  loader_copy_addr + (recieve_kernel - LOADER_START_ADDR);
    recieve_kernel_copied((char*)load_addr_ul, kernel_size, loader_copy_addr);
}