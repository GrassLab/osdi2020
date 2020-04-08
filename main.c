#include "include/uart.h"
#include "include/power.h"
#include "include/time.h"
#include "include/info.h"
#include "include/lfb.h"

int LOADIMG = 0;
int SPLASH_ON = 0;
unsigned int KERNEL_ADDR = 0x100000;

void loadimg(unsigned int kernel_addr)
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
        uart_puts("SE");
        return;
    }
    uart_puts("OK");

    uart_puts("Please input the kernel load address (default: 0x80000):\n");
    uart_puts("Please send the kernel from UART...\n");


    uart_puts("Loading kernel at 0x");
    uart_hex(kernel_addr);
    uart_puts(" with size 0x");
    uart_hex(size);
    uart_puts(" ...\n");
    
    // read the kernel
    while(size--) *(char *)kernel_addr++ = uart_getc();
}

void main()
{
    // set up serial console and linear frame buffer
    uart_init();

    // display a pixmap
    if (SPLASH_ON == 1) {
        lfb_init();
        lfb_showpicture();
    }   

    // get hardware info
    get_serial();
    get_board_revision();
    get_vccore_addr();

    while (1) {
        // get user input
        char user_input[12];
        char tmp;
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
         ** <hello> Echo hello
         */
        if (uart_strcmp(user_input, "hello") == 0) {
            uart_puts("Hello World!\n");
            continue;
        }
        /*
         ** <reboot> reboot in given cpu ticks
         */
        if (uart_strcmp(user_input, "reboot") == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n\n");
            reset(10);
            continue;
        }
        /*
         ** <time> get the timestamp
         */
        if (uart_strcmp(user_input, "time") == 0) {
            get_time();
            continue;    
        }
        /*
         ** <loadimg> listen for raspbootcom and load the img from UART
         */
        if (uart_strcmp(user_input, "loadimg") == 0) {
            uart_puts("loading image from uart...\n");
            loadimg(KERNEL_ADDR);
            LOADIMG = 1;
            break;    
        }
        /*
         ** <help> list the existed commands
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
         ** <exc> test exception
         */
        if (uart_strcmp(user_input, "exc_svc") == 0) {
            uart_puts("execute `svc #1`\n");
            asm volatile ("svc #1");
            uart_puts("done\n");
            continue;
        }
        if (uart_strcmp(user_input, "exc_brk") == 0) {
            uart_puts("execute `brk #1`\n");
            asm volatile ("brk #1");
            uart_puts("done\n");
            continue;
        }
        if (uart_strcmp(user_input, "exc_data") == 0) {
            uart_puts("execute `bad address access`\n");
            unsigned int r;
            // generate a Data Abort with a bad address access
            r=*((volatile unsigned int*)0xFFFFFFFFFF000000);
            // make gcc happy about unused variables :-)
            r++;
            uart_puts("done\n");
            continue;
        }
        /*
         ** <irq> test interrupt
         */
        if (uart_strcmp(user_input, "irq") == 0) {
            uart_puts("not implement\n");
            continue;
        }
        /*
         ** Invalid command
         */
        uart_puts("Error: command ");
        uart_puts(user_input);
        uart_puts(" not found, try <help>.\n");
    }

    if (LOADIMG == 1) {
        unsigned int stack_pointer;; 
        asm volatile ("mov %0, sp" : "=r"(stack_pointer));
        uart_puts("stack pointer: 0x");
        uart_hex(stack_pointer);
        uart_puts("\n");

        unsigned int program_counter;; 
        asm volatile ("mov %0, x30" : "=r"(program_counter));
        uart_puts("program counter: 0x");
        uart_hex(program_counter);
        uart_puts("\n");

        asm volatile (
            // we must force an absolute address to branch to
            "mov x30, %0; ret" :: "r"(KERNEL_ADDR)
        );
    }
}
