#include "io.h"
#include "shell.h"
#include "mini_uart.h"

char *welcome = \
   "              _                          " NEWLINE
   "__      _____| | ___ ___  _ __ ___   ___ " NEWLINE
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" NEWLINE
" \\ V  V /  __/ | (_| (_) | | | | | |  __/" NEWLINE
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|" NEWLINE;

int main(void){
    uart_init();
    while(1){
        println(NEWLINE, welcome);
        flush();
        shell_loop(); 
    }
    return 0;
}
