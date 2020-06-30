#include "../include/vfs.h"
#include "../include/kernel.h"

struct mount *rootfs;

int register_filesystem(struct filesystem *fs)
{
	fs->setup_mount(fs, rootfs);
	print("Register filesystem %s\n", rootfs->dentry->name);
	return 0;
}
struct file fd_create;
struct file *vfs_open(const char *pathname, int flags)
{
	struct vnode *vnode;
	struct vnode *target;
	vnode = rootfs->root;
	int found = vnode->v_ops->lookup(rootfs->dentry, &target, pathname);
	if (found == 0 && flags == O_CREAT) {
		vnode->v_ops->create(rootfs->dentry, &target, pathname);
	} else if (found == 0) {
		return NULL;
	}
	struct file *fd = &fd_create;
	fd->vnode = target;
	fd->f_ops = target->f_ops;
	fd->f_pos = 0;
	return fd;
}

int vfs_close(struct file *file)
{
	// fake do nothing
	return 0;
}

int vfs_write(struct file *file, const void *buf, int len)
{
	return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file *file, void *buf, int len)
{
	return file->f_ops->read(file, buf, len);
}