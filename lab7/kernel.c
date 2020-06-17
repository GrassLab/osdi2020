#include "MiniUart.h"
#include "shell.h"
#include "mailbox.h"
#include "exception/timer.h"
#include "schedule/task.h"
#include "schedule/schedule.h"
#include "mmu/vma.h"
#include "mmu/buddy.h"
#include "mmu/kmalloc.h"
#include "mmu/slab.h"
#include "fs/vfs.h"
#include "fs/tmpfs.h"

void kernel_main(void) {
    initUART();
    sendStringUART("Hello World!\n");

    sendStringUART("Board Revision: ");
    sendHexUART(getBoardRevision());
    sendUART('\n');

    sendStringUART("VC Core Base Addr: ");
    sendHexUART(getVCBaseAddress());
    sendUART('\n');

    gBuddy.construct();
    gSlab.init();
    kmalloc_init();

    // FIXME: shouldn't ignore return value
    register_filesystem(newTmpFs());

    rootfs = kmalloc(sizeof(struct mount));
    rootfs->fs = get_filesystem("tmpfs");
    // TODO
    rootfs->fs->setup_mount(rootfs->fs, rootfs);

    struct file *a = vfs_open("hello", 0);
    if (!(a == NULL)) {
        sendStringUART("You should not pass!!! (a == NULL)\n");
        return;
    }
    a = vfs_open("hello", O_CREAT);
    if (!(a != NULL)) {
        sendStringUART("You should not pass!!! (a != NULL)\n");
        return;
    }
    vfs_close(a);
    struct file *b = vfs_open("hello", 0);
    if (!(b != NULL)) {
        sendStringUART("You should not pass!!! (b != NULL)\n");
        return;
    }
    vfs_close(b);
    // end TODO

    rootfs->fs->unset_mount(rootfs);
    kfree(rootfs);
    unregister_filesystem();
    kmalloc_fini();
    gSlab.fini();

    sendStringUART("Press enter to continue...");
    sendUART(recvUART());

    while (1)
        ;

    initPageFrames();

    initIdleTaskState();

    sendStringUART("Want to see process of page reclaim? [y/n]\n");
    if (recvUART() == 'y') {
        page_counter_flag = 1lu;
        createPrivilegeTask(fooTask, test_command1);
        createPrivilegeTask(fooTask, test_command2);
        createPrivilegeTask(fooTask, test_command3);
    } else {
        createPrivilegeTask(fooTask, test_command1);
        createPrivilegeTask(fooTask, test_command2);
    }

    _enable_core_timer();

    idle();
}
