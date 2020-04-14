#include "MiniUart.h"
#include "shell.h"
#include "mailbox.h"
#include "exception/irq.h"

void kernel_main(void) {
    initUART();
    enable_irq();
    sendStringUART("Hello World!\n");

    sendStringUART("Board Revision: ");
    sendHexUART(getBoardRevision());
    sendUART('\n');

    sendStringUART("VC Core Base Addr: ");
    sendHexUART(getVCBaseAddress());
    sendUART('\n');

    while (1) {
        // sendUART(recvUART());
        shell();
    }
}
