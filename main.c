#include "uart.h"
#include "pcsh.h"

void system_start(){
    print("-------------------------\n");
    print("Raspberry Pi 3B+ is start\n");
    print("-------------------------\n");
}

int main(){

    // set uart
    uart_init();

    system_start();

    // call simple shell
    pcsh();

}
