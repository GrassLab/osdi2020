#include "uart.h"
#include "pcsh.h"
#include "screen.h"

void system_start(){
    print("-------------------------\n");
    print("Raspberry Pi 3B+ is start\n");
    print("-------------------------\n");
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

}
