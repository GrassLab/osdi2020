#include "bootloader.h"

#include "uart.h"

#define LOAD_ADDRESS (void *)0x100000

void load_kernel(int kernel_size){
    char *ptr = LOAD_ADDRESS;
    for(int i=0; i<kernel_size; i++){
        *ptr = uart_recv();
        ptr++;
    }
    void (*pseudo_start)(void) = LOAD_ADDRESS;
    pseudo_start();
}