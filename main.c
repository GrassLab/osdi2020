#include "include/uart.h"
#include "include/power.h"
#include "include/time.h"
#include "include/info.h"
//#include "include/lfb.h"

int LOADIMG = 0;
char *KERNEL_ADDR=(char*)0x100000;

void loadimg(char *kernel_addr)
{
    unsigned int size=0;

    // magic number
    uart_puts("HANK0438\n");
    uart_puts("Please send the kernel size...\n");
    // read the kernel's size
    size=uart_getc();
    size|=uart_getc()<<8;
    size|=uart_getc()<<16;
    size|=uart_getc()<<24;

    // send negative or positive acknowledge
    if(size<64 || size>1024*1024) {
        // size error
        uart_send('S');
        uart_send('E');
        return;
    }
    uart_send('O');
    uart_send('K');

    uart_puts("Please input the kernel load address (default: 0x80000):\n");
    uart_puts("Please send the kernel from UART...\n");


    uart_puts("Loading kernel at 0x");
    uart_hex((unsigned int)kernel_addr);
    uart_puts(" with size 0x");
    uart_hex(size);
    uart_puts(" ...\n");
    
    // read the kernel
    while(size--) *kernel_addr++ = uart_getc();
}

void main()
{
    // set up serial console and linear frame buffer
    uart_init();
    //lfb_init();

    // display a pixmap
    //lfb_showpicture();

    get_serial();
    get_board_revision();
    get_vccore_addr();

    while (1) {
        char tmp;
        char user_input[12];
        int i = 0;
        uart_send('>');
        while (i < 10) {
            tmp = uart_getc();
            if (tmp == '\n') break;
            uart_send(tmp);
            user_input[i++] = tmp;
        }
        user_input[i] = '\0';
        uart_puts("\n");
        //uart_send(uart_i2c(i));
        if (i == 0) {
            continue;
        }
        /*
         * <hello> Echo hello
         */
        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("Hello World!\n");
            continue;
        }
        /*
         * <reboot> reboot in given cpu ticks
         */
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n\n");
            reset(10);
            continue;
        }
        /*
         * <time> get the timestamp
         */
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            continue;    
        }
        /*
         * <loadimg> listen for raspbootcom and load the img from UART
         */
        if (uart_strcmp(user_input, "loadimg") == 0) {
            uart_puts("loading image from uart...\n");
            loadimg(KERNEL_ADDR);
            LOADIMG = 1;
            break;    
        }
        /*
         * <help> list the existed commands
         */
        if (uart_strcmp(user_input, "help") == 0) {
            uart_puts("hello: print hello world.\n");
            uart_puts("help: help.\n");
            uart_puts("reboot: restart.\n");
            uart_puts("time: show timestamp.\n");
            uart_puts("loadimg: reload image from UART.\n");
            continue;
        }
        /*
         * Invalid command
         */
        uart_puts("Error: command ");
        uart_puts(user_input);
        uart_puts(" not found, try <help>.\n");
    }

    if (LOADIMG == 1) {
        // restore arguments and jump to the new kernel.
        asm volatile (
            // we must force an absolute address to branch to
            "mov x30, 0x100000; ret"
        );
    }
}
