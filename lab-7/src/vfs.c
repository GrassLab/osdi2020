#include "vfs.h"
#include "allocator.h"

int register_filesystem(FileSystem *fs) {
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    rootfs = (Mount *) malloc(sizeof(Mount));
    rootfs->fs = fs;
    fs->setup_mount(fs, rootfs);
}


int next_component_name(char *path) {
    // return end index of next component name
    // -1 if fail
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/') return i
    }
}

File* vfs_open(const char *pathname, int flags) {
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.

    VNode *rootVnode = rootfs->root;
    VNode *target;
    // rootVnode->lookup(rootVnode, &target, component_name);

    // int pathnameCount = 0;
    // int componentNameCount = 0;
    // while (pathname[pathnameCount] != ) {

    // }

    if (flags == O_CREAT) {
        
    }
}

int vfs_close(File *file) {
    // 1. release the file descriptor
}

int vfs_write(File *file, const void *buf, int len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
}

int vfs_read(File *file, void *buf, int len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
}