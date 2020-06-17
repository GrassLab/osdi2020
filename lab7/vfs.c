#include "obj_alloc.h"
#include "vfs.h"
#include "string.h"
#include "tmpfs.h"
#include "uart.h"

#define NULL 0

void set_dentry(struct dentry *dentry, struct vnode *vnode,
                const char *str)
{

    dentry->child_count = 0;
    int get = varied_size_alloc(sizeof(struct dentry) * MAX_CHILD);
    struct dentry *child = (struct dentry *)fix_object_array[get].obj;
    child->objid = get;
    dentry->child_dentry = child;
    dentry->vnode = vnode;
    strcpy(dentry->dname, str);
}

void rootfs_init()
{
    int get = varied_size_alloc(sizeof(struct filesystem));
    struct filesystem *fs = (struct filesystem *)fix_object_array[get].obj;
    fs->objid = get;
    fs->name = "tmpfs";

    register_filesystem(fs);
}

int register_filesystem(struct filesystem *fs)
{
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.

    if (strcmp(fs->name, "tmpfs") != 0)
    {

        // init vops and fops

        tmpfs_v_ops = (struct vnode_operations *)fix_object_array[varied_size_alloc(sizeof(struct vnode_operations))].obj;
        tmpfs_f_ops = (struct file_operations *)fix_object_array[varied_size_alloc(sizeof(struct file_operations))].obj;

        tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;
        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs;
        int get = varied_size_alloc(sizeof(struct vnode));
        struct vnode *vnode = (struct vnode *)fix_object_array[get].obj;
        vnode->objid = get;
        set_tmpfs_vnode(vnode);

        get = varied_size_alloc(sizeof(struct dentry));
        struct dentry *dentry = (struct dentry *)fix_object_array[get].obj;
        dentry->objid = get;
        set_dentry(dentry, vnode, "/");

        // setup root file sysystem
        get = varied_size_alloc(sizeof(struct mount));
        struct mount *mt = (struct mount *)fix_object_array[get].obj;
        mt->objid = get;
        mt->fs = fs;
        mt->root = vnode;
        mt->dentry = dentry;

        rootfs = mt;
        return 0;
    }
    return 0;
}

struct file *vfs_open(const char *pathname, int flags)
{
    struct vnode *target;
    int ret = rootfs->root->v_ops->lookup(rootfs->dentry, &target, pathname);
    struct file *fd;
    if (ret == -1)
    {
        printf("not found\n");
        // not found
        if (flags == O_CREAT)
        {
            printf("create file\n");
            rootfs->root->v_ops->create(rootfs->dentry, &target, pathname);
            int get = varied_size_alloc(sizeof(struct file));
            fd = (struct file *)fix_object_array[get].obj;
            fd->objid = get;
            fd->vnode = target;
            fd->f_ops = target->f_ops;
            fd->f_pos = 0;
        }
        else
        {
            fd = NULL;
        }
    }
    else
    {
        printf("found file\n");
        int get = varied_size_alloc(sizeof(struct file));
        fd = (struct file *)fix_object_array[get].obj;
        fd->objid = get;
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
    }
    return fd;
}

int vfs_close(struct file *file)
{
    file->vnode = NULL;
    obj_free(file->objid);
    return 0;
}

int vfs_write(struct file *file, const void *buf, unsigned long len)
{
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, unsigned long len)
{
    return file->f_ops->read(file, buf, len);
}