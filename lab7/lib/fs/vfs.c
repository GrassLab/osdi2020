#include "fs/vfs.h"
#include "mmu/kmalloc.h"
#include "StringUtils.h"

struct mount* rootfs;

struct filesystem_list {
    struct filesystem *fs;
    struct filesystem_list *next;
};

// itself doesn't contain a fs
struct filesystem_list root_fs_list;

int register_filesystem(struct filesystem* fs) {
    struct filesystem_list *fs_list = kmalloc(sizeof(struct filesystem_list));
    
    fs_list->fs = fs;
    fs_list->next = root_fs_list.next;

    root_fs_list.next = fs_list;
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.

    return 0;
}

void unregister_filesystem(void) {
    struct filesystem_list *fs_list = root_fs_list.next;

    while (fs_list) {
        struct filesystem_list *cur = fs_list;

        kfree(cur->fs);
        kfree(cur);

        fs_list = fs_list->next;
    }
}

struct filesystem *get_filesystem(const char *name) {
    struct filesystem_list *fs_list = root_fs_list.next;

    while (fs_list) {
        if (compareString(fs_list->fs->name, name) == 0) {
            return fs_list->fs;
        }
        fs_list = fs_list->next;
    }
    return NULL;
}

struct file* vfs_open(const char* pathname, int flags) {
    // 1. Lookup pathname from the root vnode.

    // FIXME: assume pathname will be only one component_name
    struct vnode *target = NULL;
    rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);
    if (!target && !(flags & O_CREAT)) {
        return NULL;
    }

    // 2. Create a new file descriptor for this vnode if found.

    struct file *opened_file = kmalloc(sizeof(struct file));
    opened_file->vnode = target;
    opened_file->f_pos = 0;
    // TODO: f_ops
    opened_file->flags = flags;

    // 3. Create a new file if O_CREAT is specified in flags.
    if (!target && (flags & O_CREAT)) {
        // FIXME: check return value
        rootfs->root->v_ops->create(rootfs->root, &target, pathname);
        opened_file->vnode = target;
    }
    
    return opened_file;
}
int vfs_close(struct file* file) {
    kfree(file);

    // FIXME: unknown return value meaning
    return 0;
}
int vfs_write(struct file* file, const void* buf, size_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, size_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
}
