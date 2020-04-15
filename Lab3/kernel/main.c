#include "uart.h"
#include "shell.h"

void main() {


    uart_init();
    mboxinfo("SER");
    mboxinfo("VC");
    mboxinfo("BREV");
    uart_puts("\nThe mini shell is ready.\n\r");
    while(1) {
        wait_command();
    }
}