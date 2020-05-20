#include "MiniUart.h"
#include "shell.h"
#include "mailbox.h"
#include "exception/timer.h"
#include "schedule/task.h"
#include "schedule/schedule.h"
#include "mmu/vma.h"

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

    initPageFrames();

    initIdleTaskState();

    _enable_core_timer();

    createPrivilegeTask(fooTask, test_command1);
    createPrivilegeTask(fooTask, test_command2);

    idle();
}
