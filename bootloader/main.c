#include "uart.h"
#include "string.h"
#include "bootloader.h"
#define INPUT_BUFFER_SIZE 64

void system_start()
{
    uart_puts("---------------------------\n");
    uart_puts("Raspberry Pi 3B+ Bootloader\n");
    uart_puts("---------------------------\n");
}

int main()
{
    // set uart
    uart_init();

    system_start();

    char cmd[INPUT_BUFFER_SIZE];
    while (1)
    {
        uart_puts("\r# ");
        
        memset(cmd, 0, INPUT_BUFFER_SIZE);
        uart_gets(cmd, INPUT_BUFFER_SIZE);
        
        uart_send('\r');
        uart_send('\n');

        if (strcmp(cmd, "") == 0)
            continue;
        else if(strcmp(cmd, "hello") == 0)
            uart_puts("Hello World!\n");
        else if(strcmp(cmd, "load_images") == 0){
            loadimg();
        }
        else{
            uart_puts("Not find\n");
        }
    }

    return 0;
}
