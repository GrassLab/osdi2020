#include "MiniUart.h"
#include "shell.h"
#include "mailbox.h"
#include "schedule/task.h"
#include "schedule/schedule.h"

void idle(void) {
    while (1) {
        sendStringUART("Enter idle state ...\n");
        delay(0x3000000u);
        schedule();
    }
}

void kernel_main(void) {
    initUART();
    sendStringUART("Hello World!\n");

    sendStringUART("Board Revision: ");
    sendHexUART(getBoardRevision());
    sendUART('\n');

    sendStringUART("VC Core Base Addr: ");
    sendHexUART(getVCBaseAddress());
    sendUART('\n');

    sendStringUART("Press enter to continue...");
    sendUART(recvUART());

    initIdleTaskState();

    for (uint32_t i = 0; i < 3; ++i) {
        createPrivilegeTask(fooTask);
    }

    idle();
}
