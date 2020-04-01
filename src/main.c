#include "lfb.h"
#include "mailbox.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    uart_init();    
    get_board_info();

    lfb_init();
    // // display a pixmap
    lfb_showpicture();
    
    run_shell();
    return -1;
}
