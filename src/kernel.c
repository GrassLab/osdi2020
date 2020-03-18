#include "io.h"
#include "shell.h"
#include "mini_uart.h"

char *welcome = 
"              _                          "     NEWLINE
"__      _____| | ___ ___  _ __ ___   ___ "     NEWLINE
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" NEWLINE
" \\ V  V /  __/ | (_| (_) | | | | | |  __/"       NEWLINE
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|"  NEWLINE;

int main(void){
#ifdef TEST
    int *beg, *end;
    println("SHOW ALL BSS VALUE")
    __asm__ volatile ("adr %0, __bss_beg" : "=r"(beg));
    __asm__ volatile ("adr %0, __bss_end" : "=r"(end));
    for(int *p = beg; p != end; p++){
        print("0x");
        _print_ULL_as_number((unsigned long long)p, 16);
        println(": ", (int)*p);
    }
#endif

    uart_init();
    while(1){
#ifndef TEST
        println(NEWLINE, welcome);
#endif
        flush();
        shell_loop(); 
    }
    return 0;
}
