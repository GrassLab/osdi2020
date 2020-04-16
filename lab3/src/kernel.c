#include "uart.h"
#include "mystd.h"

#define LOADER_START_ADDR 0x80000
#define LOADER_SIZE_MAX 0x10000

extern char __loader_size[];
unsigned int loader_size;
char *gpu_load_addr ;

unsigned int get_kernel_size(){
    unsigned int size;
    size = uart_getc();
    size |= uart_getc()<<8;
    size |= uart_getc()<<16;
    size |= uart_getc()<<24;
    return size;
}

unsigned long calc_loader_copy_addr(unsigned long load_kernel_addr){
    unsigned long copy_addr;
    loader_size = (unsigned long)__loader_size;
    // uart_dec(loader_size); uart_puts("\n");
    // unsigned int stack_reserve = 1024;
    copy_addr = load_kernel_addr - LOADER_SIZE_MAX;
    // uart_dec(copy_addr); uart_puts("\n");
    return copy_addr;
}

void recieve_kernel(unsigned long load_addr, unsigned int size){
    // uart_puts("recieve start...\n");
    char *kernel=(char*)load_addr;
    
    // important!!! the ori uart_getc would be overwirte, too
    char (*uart_getc_copied)() = calc_loader_copy_addr(load_addr) + (uart_getc - LOADER_START_ADDR); 
    
    while(size--) 
        *kernel++ = uart_getc_copied();
    // uart_puts("recieve done...\n");
   
    asm volatile ("mov x30, %0; ret" ::"r"(load_addr)); 
    //should return immediately in asm line above or the compiler will load other address to x30 then retrun
}

void copy_loader_and_jump(char *copy_addr, char *load_kernel_addr, unsigned int kernel_size){
    gpu_load_addr = (char*)LOADER_START_ADDR;

    //copy loader
    char *loader_old_base = gpu_load_addr;
    char *loader_new_base = copy_addr;
    unsigned int loader_byte = loader_size ;
    // uart_dec(loader_byte); uart_puts("\n");
    while(loader_byte--)
        *loader_new_base++ = *loader_old_base++;
    // uart_hex((unsigned long)loader_old_base); uart_puts("\n");
    // uart_hex((unsigned long)loader_new_base); uart_puts("\n");

    //copy stack
    char *old_stack = gpu_load_addr-1;
    char *new_stack = copy_addr-1;
    char* sp;
    asm volatile ("mov %0, sp" :"=r"(sp));
    // uart_dec(LOADER_START_ADDR-(unsigned long)sp); uart_puts("\n");
    while(old_stack >= sp)
        *new_stack-- = *old_stack--;
    sp = new_stack + 1;
    asm volatile ("mov sp, %0" ::"r"(sp));
    
    asm volatile ("sub x30, x30, #0x80000;"
                  "add x30, x30, %0"::"r"(copy_addr)); 

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
    // uart_hex(addr_ul);
    // uart_puts("\n");

    unsigned long loader_copy_addr;
    loader_copy_addr = calc_loader_copy_addr(load_addr_ul);
    copy_loader_and_jump((char*)loader_copy_addr, (char*)load_addr_ul, kernel_size);
    uart_puts("\nloading kernel image...\n\n\n");
    recieve_kernel(load_addr_ul, kernel_size);
}