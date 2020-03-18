#include "io.h"
#include "shell.h"
#include "mini_uart.h"

#ifdef TEST
#include "test.h"
#endif

char *welcome = 
"              _                          "     NEWLINE
"__      _____| | ___ ___  _ __ ___   ___ "     NEWLINE
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" NEWLINE
" \\ V  V /  __/ | (_| (_) | | | | | |  __/"       NEWLINE
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|"  NEWLINE;

int main(void){

    uart_init();

#ifdef TEST
    puts("");
    ASSERT(bss_all_be() == 0);
    ASSERT((unsigned long long)stack_pointer() < (unsigned long long)0x80000);
#endif
    while(1){
        println(NEWLINE, welcome);
        flush();
        shell_loop(); 
    }
    return 0;
}
