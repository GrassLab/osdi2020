#include "frame_buffer.h"
#include "mailbox.h"
#include "simple_shell.h"
#include "uart.h"
#include "string.h"

int main()
{
    int r=15000000; while(r--) { asm volatile("nop"); }
    uart_init();    
    fbuff_init();
    get_board_info();
    // display a pixmap
    fbuff_showBitmap();

    run_shell();
    return -1;
}
