#include "tmpfs.h"
#include "vfs.h"
#include "allocator.h"

#define NULL 0

void tmpfs_vnode_init(VNode *vnode) {
    vnode->v_ops->lookup = tmpfs_lookup;
    vnode->v_ops->create = tmpfs_create;
    vnode->v_ops->mkdir = tmpfs_mkdir;
    vnode->f_ops->write = tmpfs_write;
    vnode->f_ops->read = tmpfs_read;
}

void dentry_init(Dentry *dentry, char *name, int flag) {
    dentry->flag = flag;
    dentry->childCount = 0;
    dentry->parentDentry = NULL;
    strcpy(dentry->name, name);
}

int tmpsfs_setup_mount(FileSystem *fs, Mount *mount) {
    VNode *vnode = (VNode*) malloc (sizeof(VNode));
	Dentry *dentry = (Dentry*) malloc (sizeof(Dentry));
    TmpfsNode *tmpfsNode = (TmpfsNode*) malloc (sizeof(TmpfsNode));

    tmpfs_vnode_init(vnode);
    dentry_init(dentry, "/", ROOT_DIR);

    vnode->internal = tmpfsNode;
    tmpfsNode->dentry = dentry;
	
    mount->fs = fs;
	mount->root = vnode;
	return 0;
}

int tmpfs_lookup(VNode *dir_node, VNode **target, const char *component_name) {

}

int tmpfs_create(VNode *dir_node, VNode **target, const char *component_name) {

}

int tmpfs_mkdir(VNode *dir_node, VNode **target, const char *component_name) {

}

int tmpfs_write(File *file, const void *buf, int len) {

}

int tmpfs_read(File *file, void *buf, int len) {

}
