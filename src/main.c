#include "../include/uart.h"
#include "../include/power.h"
#include "../include/time.h"
#include "../include/info.h"
#include "../include/lfb.h"
#include "../include/loadimg.h"
#include "../include/cmdline.h"
#include "../include/interrupt.h"
//#include "../include/exception.h"

#define WELCOME \
    "                                                 \n" \
    "   ____   _____ _____ _____ ___   ___ ___   ___  \n" \
    "  / __ \\ / ____|  __ \\_   _|__ \\ / _ \\__ \\ / _ \\ \n" \
    " | |  | | (___ | |  | || |    ) | | | | ) | | | |\n" \
    " | |  | |\\___ \\| |  | || |   / /| | | |/ /| | | |\n" \
    " | |__| |____) | |__| || |_ / /_| |_| / /_| |_| |\n" \
    "  \\____/|_____/|_____/_____|____|\\___/____|\\___/ \n" \
    "                                                 \n"

int SPLASH_ON = 0;
int LOADIMG = 0;
unsigned int KERNEL_ADDR = 0x100000;
unsigned int CORE_TIMER_COUNT = 0;
unsigned int LOCAL_TIMER_COUNT = 0;

void main123()
{
    uart_irq_enable();
    uart_init();
    for (int i=0; i<16; i++)
        uart_send('a');
    uart_send('\r');
    uart_send('\n');
    while(1);
}

void main()
{
    // set up serial console and linear frame buffer
    uart_irq_enable();
    uart_init();
    uart_puts(WELCOME);
    // char *int1 = 0;
    // uart_atoi(int1, 123456);
    // uart_puts(int1);
    // char *int2 = 0;
    // uart_atoi(int2, -13456);
    // uart_puts(int1);

    // display a pixmap
    if (SPLASH_ON == 1) {
        lfb_init();
        lfb_showpicture();
    }   

    // get hardware info
    show_serial();
    show_board_revision();
    show_vccore_addr();

    while (1) { 
        // get user input
        // command line
        char user_input[256];
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
        //read_cmdline(user_input);
        /*
         ** <hello> Echo hello
         */
        if (uart_strncmp(user_input, "hello", 5) == 0) {
            uart_puts("Hello World!\n");
        }
        /*
         ** <reboot> reboot in given cpu ticks
         */
        else if (uart_strncmp(user_input, "reboot", 6) == 0) {
            get_time();
            uart_puts("reboot in 10 ticks.\n\n");
            reset(10);
        }
        /*
         ** <time> get the timestamp
         */
        else if (uart_strncmp(user_input, "time", 4) == 0) {
            get_time(); 
        }
        /*
         ** <loadimg> listen for raspbootcom and load the img from UART
         */
        else if (uart_strncmp(user_input, "loadimg", 7) == 0) {
            uart_puts("loading image from uart...\n");
            loadimg(KERNEL_ADDR);
            LOADIMG = 1;
            break;    
        }
        /*
         ** <help> list the existed commands
         */
        else if (uart_strncmp(user_input, "help", 4) == 0) {
            uart_puts("hello: print hello world.\n");
            uart_puts("help: help.\n");
            uart_puts("reboot: restart.\n");
            uart_puts("time: show timestamp.\n");
            uart_puts("loadimg: reload image from UART.\n");
            uart_puts("exc_svc: execute `svc #1`.\n");
            uart_puts("exc_brk: execute `brk #0`.\n");
            uart_puts("irq_core: core timer interrupt.\n");
            uart_puts("irq_local: local timer interrupt.\n");
        }
        /*
         ** <exc> test exception
         */
        else if (uart_strncmp(user_input, "exc_svc", 7) == 0) {
            uart_puts("execute `svc #1`\n");
            asm volatile ("svc #1");
            uart_puts("done\n");
        }
        else if (uart_strncmp(user_input, "exc_brk", 7) == 0) {
            uart_puts("execute `brk #0`\n");
            asm volatile ("brk #0");
            uart_puts("done\n");
        }
        /*
         ** <irq> test interrupt
         */
        else if (uart_strncmp(user_input, "irq_core", 8) == 0) {
            uart_puts("enable core timer.\n");
            //core_timer_enable();
            core_timer_enable_user();
        }
        else if (uart_strncmp(user_input, "irq_local", 9) == 0) {
            uart_puts("enable local timer.\n");
            local_timer_init();
        }
        /*
        ** Invalid command
        */
        else {
            uart_puts("Error: command ");
            uart_puts(user_input);
            uart_puts(" not found, try <help>.\n");
        }
    }
    if (LOADIMG == 1) {
        // unsigned int stack_pointer;; 
        // asm volatile ("mov %0, sp" : "=r"(stack_pointer));
        // uart_puts("stack pointer: 0x");
        // uart_hex(stack_pointer);
        // uart_puts("\n");

        // unsigned int program_counter;; 
        // asm volatile ("mov %0, x30" : "=r"(program_counter));
        // uart_puts("program counter: 0x");
        // uart_hex(program_counter);
        // uart_puts("\n");
        /*
        ** Start to load image
        */
        asm volatile (
            // we must force an absolute address to branch to
            "mov x30, %0; ret" :: "r"(KERNEL_ADDR)
        );
    }
    
}
