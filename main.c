#include "uart.h"
#include "pcsh.h"

int main(){

    // set uart
    uart_init();


    // call simple shell
    pcsh();

}
