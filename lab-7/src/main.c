#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "printf.h"
#include "memory.h"
#include "allocator.h"
#include "tmpfs.h"
#include "vfs.h"

void init_rootfs() {
    tmpfs = (FileSystem *) malloc(sizeof(FileSystem));
    tmpfs->name = "tmpfs";
    tmpfs->setup_mount = tmpsfs_setup_mount;
    register_filesystem(tmpfs);
}

int main()
{
    uart_init();
    init_printf(0, putc);
    page_sys_init();
    allocator_init();
    varied_size_init();
    init_rootfs();
}
