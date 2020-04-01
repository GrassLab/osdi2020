#include "lfb.h"
#include "mailbox.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    uart_init();
    get_board_revision();
    get_ARM_address();
    get_VC_address();    

    lfb_init();
    // // display a pixmap
    lfb_showpicture();

    run_shell();
    return -1;
}
