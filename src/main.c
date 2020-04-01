#include "frame_buffer.h"
#include "mailbox.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    uart_init();    
    fbuff_init();
    get_board_info();
    // // display a pixmap
    fbuff_showBitmap();

    run_shell();
    return -1;
}
