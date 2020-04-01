#include "uart.h"
#include "shell.h"
#include "framebuffer.h"

int main()
{
    // set up serial console
    uart_init();

    // set up framebuffer
    framebuffer_init ();
    framebuffer_show_pic ();
    
    // say hello
    uart_puts("Hello World!\n");

    // start shell
    shell_start();
    
    return 0;
}