#include "MiniUart.h"
#include "shell.h"

void kernel_main(void) {
    initUART();
    sendStringUART("Hello World!\n");

    while (1) {
        // sendUART(recvUART());
        shell();
    }
}
