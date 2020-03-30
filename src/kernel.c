#include "io.h"
#include "shell.h"
#include "uart.h"
#include "lfb.h"
#include "mbox.h"
#include "info.h"

#ifdef TEST
#include "test.h"
#endif

char *welcome = 
"              _                          "     NEWLINE
"__      _____| | ___ ___  _ __ ___   ___ "     NEWLINE
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" NEWLINE
" \\ V  V /  __/ | (_| (_) | | | | | |  __/"       NEWLINE
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|"  NEWLINE;

char *_kend;
char *_kbeg;
int main(void){

    uart_init();

    lfb_init();
    lfb_showpicture();

#ifdef TEST
    puts("");
    ASSERT(bss_all_be() == 0);
    ASSERT((unsigned long long)stack_pointer() < (unsigned long long)0x80000);
#endif
    while(1){
        println(NEWLINE, welcome);

#ifdef BUILD_STAMP
#define xstr(a) str(a)
#define str(a) #a
        println(" BUILD @ ", xstr(BUILD_STAMP));
#endif
        printf("   kerned segment: 0x%x - 0x%x" NEWLINE NEWLINE,
                &_kbeg, &_kend);

        println("UART TYPE: ", UART_TYPE);
        if(get_board_revision()) printf("Board revision: %x" NEWLINE, mbox[5]);
        if(get_arm_memaddr()) printf("ARM base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
        if(get_vc_memaddr()) printf("VC Core base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
        puts("");
        flush();
        shell_loop(); 
    }
    return 0;
}
