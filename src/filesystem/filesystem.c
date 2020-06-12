#include "utils.h"
#include "memory/memPool.h"
#include "filesystem/filesystem.h"
#include "filesystem/tmpfs.h"
#include "device/uart.h"

struct mount *rootfs;

int32_t registerFilesystem(struct filesystem* fs) 
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.

    fs->vnode_token = getFreePool(sizeof(struct vnode));
    fs->file_token = getFreePool(sizeof(struct file));
    fs->setup_mount(fs, rootfs);

    return 0;
}

int32_t createFilesystem(const char *fsname)
{
    struct filesystem *fs = (struct filesystem *)allocDynamic(sizeof(struct filesystem));;
    copystr(fsname, fs->name);
    uartPuts("fs name: ");
    uartPuts(fs->name);
    uartPuts("\n");

    if (strcmp(fsname, "tmpfs")) 
        fs->setup_mount = tmpfsSetupMount;

    registerFilesystem(fs);
}

struct file *vfsOpen(const char* pathname, int flags) 
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.

    struct vnode **target;
    if (rootfs->root->v_ops->lookup(rootfs->root, target, pathname) == -1)
    {
        if (flags == O_CREAT)
            rootfs->root->v_ops->create(*target, target, pathname);
        else
            return (struct file *)0;
    }
        
    struct file *f = (struct file *)allocSlot(rootfs->fs->file_token);
    f->flags = flags;
    f->f_pos = 0;
    f->f_ops = rootfs->root->f_ops;
    f->vnode = target;

    return f;
}
int32_t vfsClose(struct file *file) {
     // 1. release the file descriptor
}
int32_t vfsWrite(struct file *file, const void *buf, size_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
}
int32_t vfsRead(struct file *file, void *buf, size_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
}