#include "vfs.h"
#include "tmpfs.h"
#include "printf.h"
#include "allocator.h"

#define NULL 0

int register_filesystem(FileSystem *fs) {
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    rootfs = (Mount *) malloc(sizeof(Mount));
    rootfs->fs = fs;
    fs->setup_mount(fs, rootfs);
}

File* vfs_open(const char *pathname, int flags) {
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.

    VNode *rootVnode = rootfs->root;
    VNode *tmpVnode = rootVnode;
    VNode *target;

    int result = rootVnode->v_ops->lookup(rootVnode, &target, pathname);
    if (result) {
        File *file = (File *) malloc(sizeof(File));
        file->vnode = target;
        file->f_ops = rootVnode->f_ops;
        file->f_pos = 0;
        return file;
    }

    if (flags == O_CREAT) {
        VNode *node = (VNode *) malloc(sizeof(VNode));
        File *file = (File *) malloc(sizeof(File));
        file->vnode = node;
        file->f_ops = rootVnode->f_ops;
        file->f_pos = 0;
        rootVnode->v_ops->create(rootVnode, &node, pathname);
        return file;
    }
    return NULL;
}

int vfs_close(File *file) {
    // 1. release the file descriptor
    printf("[close]\n");
    free(file);
    return 0;
}

int vfs_write(File *file, const void *buf, int len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file, buf, len);
}

int vfs_read(File *file, void *buf, int len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}

int vfs_ls(VNode *vnode) {
    vnode->v_ops->ls(vnode);
    return 1;
}

int vfs_mkdir(VNode *dir_node, char *dir_name) {
    VNode *target = (VNode *) malloc(sizeof(VNode));
    return dir_node->v_ops->mkdir(dir_node, &target, dir_name);
}