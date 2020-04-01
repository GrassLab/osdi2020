#include "uart.h"
#include "pcsh.h"
#include "screen.h"
#include "string.h"


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



    // call simple shell
    pcsh();

    return 0;
}
