#include "exec_utils.h"

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC =  (PM_PASSWORD | 0x20); // full reset
  *PM_WDOG =  (PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset(){
  *PM_RSTC = (PM_PASSWORD | 0); // full reset
  *PM_WDOG = (PM_PASSWORD | 0); // number of watchdog tick
}

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

void copy_and_jump_to_kernel() {
    unsigned long new_address = 0x80000;
    unsigned long *kernel = (unsigned long *)new_address;
    int kernel_size = recv_img_size();
    uart_send_int(kernel_size);
    unsigned long end = new_address;

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
    void (*jump_new_kernel)(void) = (void (*)(void))new_address;
    jump_new_kernel();
    // branch_to_address((unsigned long int *)new_address);
}

void copy_self_kernel() {
    unsigned long *kernel = (unsigned long *)start_begin;
    unsigned long *end = (unsigned long *)__bss_end;
    unsigned long *copy = (unsigned long *)(TMP_KERNEL_ADDR);
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("copy self kernel finish\n");
    void (*func_ptr)() = copy_and_jump_to_kernel;
    unsigned long original_function_address = (unsigned long)func_ptr;
    void (*call_function)() = (void (*)())(original_function_address - (unsigned long)start_begin + TMP_KERNEL_ADDR);
    call_function();
}