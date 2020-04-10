#include "uart.h"
#include "pcsh.h"
#include "screen.h"
#include "string.h"

#define INPUT_BUFFER_SIZE 1024

void system_start(){
    uart_print("-------------------------\n");
    uart_print("Raspberry Pi 3B+ is start\n");
    uart_print("-------------------------\n");
}

int main(){

    // set uart
    uart_init();

    system_start();

    get_board_revision();
    get_vc_memory();
    
    get_frame_buffer();

    showpicture();


    char cmd[INPUT_BUFFER_SIZE];
    // main loop
    while (1)
    {
        uart_puts(">");

        // get command
        memset(cmd, 0, INPUT_BUFFER_SIZE);
        uart_gets(cmd, INPUT_BUFFER_SIZE);

        uart_send('\r');
        uart_send('\n');

        if(strcmp(cmd, "") == 0)
            continue;
        else if(strcmp(cmd, "hello") == 0)
            cmd_hello(0);
        else if(strcmp(cmd, "reboot") == 0)
            cmd_reboot(0);
        else if(strcmp(cmd, "timestamp") == 0)
            cmd_timestamp(0);
        else if(strcmp(cmd, "load_images") == 0)
            cmd_load_images(0);
        else
            cmd_not_find(0);
            


    }
    uart_puts("Shell End\n");


    return 0;
}
