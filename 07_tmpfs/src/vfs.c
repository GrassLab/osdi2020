#include "vfs.h"
#include "mm.h"
#include "fs/tmpfs.h"
#include "utils.h"
#include "peripherals/uart.h"

struct mount* rootfs;
struct dentry* current_dent;
char mt_table_map[MOUNT_TABLE_SIZE];
struct mount* mount_fs_table[MOUNT_TABLE_SIZE];

void set_dentry(struct dentry *dentry, struct vnode *vnode, const char* dname) {
    dentry->child_count = 0;
    dentry->vnode = vnode;
    strcpy(dentry->dname, dname);
}

void rootfs_init() {
    struct filesystem *fs = (struct filesystem*) dynamic_alloc(sizeof(struct filesystem));
    memzero((unsigned long)fs, sizeof(struct filesystem));
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount_tmpfs;

    int ret = register_filesystem(fs);
    if (ret != 0) {
        uart_puts("register filesystem failed\n");
    }

    struct  vnode *vnode = (struct vnode*) dynamic_alloc(sizeof(struct vnode));
    set_tmpfs_vnode(vnode);
    
    struct dentry *dentry = (struct dentry*) dynamic_alloc(sizeof(struct dentry));
    set_dentry(dentry, vnode, "/");
    dentry->flag = ROOT_DIR;

    struct mount *mt = (struct mount*) dynamic_alloc(sizeof(struct mount));
    mt->fs = fs;
    mt->root = vnode;
    mt->dentry = dentry;
    rootfs = mt;

    current_dent = dentry;
}

int register_filesystem(struct filesystem *fs) {
    if (strcmp(fs->name, "tmpfs") == -1) {
        tmpfs_v_ops = (struct vnode_operations*) dynamic_alloc(sizeof(struct vnode_operations));
        tmpfs_f_ops = (struct file_operations*) dynamic_alloc(sizeof(struct file_operations));

        tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;

        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs; 

        return 0;
    }
    return -1;
}

struct file* vfs_open(const char *pathname, int flags) {
    struct dentry *dent = current_dent;
    if (flags == O_CREAT) {
        struct vnode * target;
        int ret = rootfs->root->v_ops->lookup(dent, &target, pathname);
        if (ret < 0) {
            rootfs->root->v_ops->create(dent, &target, pathname);
        } else {
            return (struct file*)NULL;
        }
        struct file* fd = (struct file*) dynamic_alloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    } else {
        struct vnode *target;
        int ret = rootfs->root->v_ops->lookup(dent, &target, pathname);

        if (ret == -1) {
            return (struct file*)NULL;
        }
        struct file* fd = (struct file*) dynamic_alloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    }        
}

int vfs_close(struct file* file) {
    dynamic_free((unsigned long)file);
    return 0;
}

int vfs_write(struct file* file, const void* buf, size_t len) {
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file* file, void* buf, size_t len) {
    return file->f_ops->read(file, buf, len);
}
