#include "MiniUart.h"
#include "shell.h"
#include "mailbox.h"

void kernel_main(void) {
    initUART();
    sendStringUART("Hello World!\n");
    sendHexUART(getBoardRevision());

    while (1) {
        // sendUART(recvUART());
        shell();
    }
}
