#include "io.h"
#include "shell.h"
#include "mini_uart.h"

char *welcome = \
   "              _                          \n"
   "__      _____| | ___ ___  _ __ ___   ___ \n"
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\\n"
" \\ V  V /  __/ | (_| (_) | | | | | |  __/\n"
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|\n";

int main(void){
    uart_init();
    while(1){
        println(welcome);
        shell_loop(); 
    }
    return 0;
}
