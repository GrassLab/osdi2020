#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED
#include "shell.h"
#define  TMP_KERNEL_ADDR  0x100000
extern char __bss_end[];
extern char start_begin[];


int recv_img_size(){
    int size = 0;
    char c;
    while (1){
        c = uart_getc();
        if(c == '\n') break;
        uart_send(c);
        uart_puts("-");
        size *= 10;
        size += (int)(c -'0');
    }
    return size;
}

void copy_and_jump_to_kernel(char *new_address) {
    new_address = 0x80000;
    char *kernel = new_address;
    int kernel_size = recv_img_size();
    uart_send_int(kernel_size);
    char *end = new_address;

    int checksum = 0;
    for (int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_recv();
        checksum += c;
        kernel[i] = c;
        end++;
    }
    uart_send_int(checksum);
    uart_puts("copy new kernel finish\n");
    uart_hex(end);
    uart_puts("\n");
    // #define LOADIMG_TEMP_LOCATION 0x10000;
    void (*jump_new_kernel)(void) = new_address;
    jump_new_kernel();

    // branch_to_address((unsigned long int *)new_address);
}

void copy_self_kernel(char *new_address) {
    char *kernel = start_begin;
    char *end = __bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("copy self kernel finish\n");
    void (*func_ptr)() = copy_and_jump_to_kernel;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)(char *) = (void (*)(char *))(original_function_address - (unsigned long int)start_begin + TMP_KERNEL_ADDR);
    call_function(new_address);
}

void shell(){
    _memset(buff, '\0', 50); 
    char *buff_ptr = buff;

    uart_puts("Welcome! osdi lab2 test lab2 bootloader img!!!\n#");
    char c = uart_getc();
    while(1) {
        c = uart_getc();
        if(c=='\n'){
            uart_puts("\n");
            if(!_compare_input(buff)){
                uart_puts("Err: Command ");
                _print(buff);
                uart_puts(" not found, try <help>\n");
            }
            uart_puts("#");
            _memset(buff, '\0', 50);    
            buff_ptr = buff;
        }
        else{
            uart_send(c);
            *buff_ptr++ = c;
        }
    }
}


void _unsign_arr_to_digit(unsigned num, char* buf, unsigned len){
	for(int i=len-1; i>=0; i--){
		buf[i] = (char)(num%10 + '0');
		num /= 10;
	}
}




#endif