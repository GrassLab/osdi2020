#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#include "timer.h"
#include "kernel.h"
#include "mm.h"
#include "buddy.h"
#define S_MAX 1000

void main(){
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
        char str[S_MAX];

        uart_puts("# ");
        read_string(str);

        if(strcmp(str,"")==0){
            ;
        }else{
            printf("Err: command %s not found, try <help>\n", str);
        }
    }

    return;
}
