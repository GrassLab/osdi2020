#include "shell.h"
#include "my_string.h"
#include "uart.h"

void run(char *command){
    if (!strcmp(command, "hello")){
        hello();
    }
    else if (!strcmp(command, "help")){
        help();
    }
    else {
        uart_puts("Error: command not found.\n");
    }
}

void help(){
    uart_puts("<hello>: print Hello World!\n");
    uart_puts("---\n");
    uart_puts("<help>: print all available commands.\n");
}

void hello(){
    uart_puts("Hello World!\n");
}
