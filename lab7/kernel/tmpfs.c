#include "vfs.h"
#include "tmpfs.h"
#include <string.h>

extern struct mount *rootfs;
struct filesystem fs;
struct mount tmpfs;

int setup_tmpfs_filesystem()
{
	// fs.name = "tmpfs";
	strcpy(fs.name, "tmpfs");
	fs.setup_mount = setup_tmpfs_mount;
	register_filesystem(&fs);
}

struct vnode vnode;
struct vnode_operations v_ops;
struct file_operations f_ops;
struct dentry dentry;
int setup_tmpfs_mount(struct filesystem *fs, struct mount *mount)
{
	tmpfs.fs = fs;
	tmpfs.root = &vnode;
	tmpfs.root->v_ops = &v_ops;
	tmpfs.root->v_ops->lookup = tmpfs_lookup;
	tmpfs.root->v_ops->create = tmpfs_create;
	tmpfs.root->f_ops = &f_ops;
	tmpfs.root->f_ops->write = tmpfs_write;
	tmpfs.root->f_ops->read = tmpfs_read;
	// print(tmpfs.root->f_ops->write == (void *)0 ? "NULL\n" : "NOT NULL\n");
	tmpfs.dentry = &dentry;
	tmpfs.dentry->vnode = tmpfs.root;
	tmpfs.dentry->count = 0;
	strcpy(tmpfs.dentry->name, "/");
	rootfs = &tmpfs;
	// print(rootfs->root->f_ops->write == (void *)0 ? "NULL\n" :
	// 						"NOT NULL\n");
	return 0;
};

int tmpfs_write(struct file *file, const void *buf, int len)
{
	struct vnode *vnode = file->vnode;
	struct tmpfs_internal *internal = vnode->internal;
	if (internal->type != FILE) {
		return -1;
	}
	strncpy(internal->content, buf, len);
	internal->content_size = len;
	return internal->content_size;
}
int tmpfs_read(struct file *file, void *buf, int len)
{
	struct vnode *vnode = file->vnode;
	struct tmpfs_internal *internal = vnode->internal;
	if (internal->type != FILE) {
		return -1;
	}
	strncpy(buf, internal->content, len);
	return len;
}
int tmpfs_lookup(struct dentry *dentry, struct vnode **target,
		 const char *component_name)
{
	struct dentry *child;
	for (int i = 0; i < dentry->count; i++) {
		child = dentry->child[i];
		if (strcmp(child->name, component_name) == 0) {
			*target = child->vnode;
			return 1;
		}
	}
	return 0;
}

struct vnode_operations v_ops_create;
struct file_operations f_ops_create;
struct vnode vnode_create = { .v_ops = &v_ops_create, .f_ops = &f_ops_create };
struct tmpfs_internal internal_create;
struct dentry child_create;
int tmpfs_create(struct dentry *dentry, struct vnode **target,
		 const char *component_name)
{
	struct vnode *vnode = &vnode_create;
	vnode->v_ops = &v_ops_create;
	vnode->v_ops->lookup = tmpfs_lookup;
	vnode->v_ops->create = tmpfs_create;
	vnode->f_ops = &f_ops_create;
	vnode->f_ops->write = tmpfs_write;
	vnode->f_ops->read = tmpfs_read;

	struct tmpfs_internal *internal = &internal_create;
	internal->type = FILE;
	internal->content_size = 0;
	vnode->internal = internal;
	struct dentry *child = &child_create;
	child->vnode = vnode;
	child->parent = dentry;

	dentry->child[dentry->count++] = child;
	child->count = 0;
	strcpy(child->name, component_name);
	*target = vnode;
}