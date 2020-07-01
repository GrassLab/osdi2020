#include "tmpfs.h"
#include "string.h"
#include "vfs.h"
#include "allocator.h"
#include "printf.h"

#define NULL 0

void tmpfs_vnode_init(VNode *vnode) {
    VNodeOperations *v_ops = (VNodeOperations *) malloc(sizeof(VNodeOperations));
    FileOperations *f_ops = (FileOperations *) malloc(sizeof(FileOperations));
    vnode->v_ops = v_ops;
    vnode->f_ops = f_ops;
    vnode->v_ops->ls = tmpfs_ls;
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
    printf("[lookup] directory name: \"%s\"\n", ((TmpfsNode *)(dir_node->internal))->dentry->name);
    Dentry *dentry = ((TmpfsNode *)(dir_node->internal))->dentry;
    for (int i = 0; i < dentry->childCount; i++) {
        if (str_equal(dentry->childDentry[i]->name, component_name)) {
            *target = dentry->childDentry[i]->vnode;
            return 1;
        }
    }
    return 0;
}

int tmpfs_create(VNode *dir_node, VNode **target, const char *component_name) {
    printf("[create] directory name: \"%s\", file name: \"%s\"\n", 
        ((TmpfsNode *)(dir_node->internal))->dentry->name, component_name);

    Dentry *dirDentry = ((TmpfsNode *)(dir_node->internal))->dentry;
    Dentry *targetDentry = (Dentry *) malloc(sizeof(Dentry));
    TmpfsNode *targetTmpfsNode = (TmpfsNode *) malloc(sizeof(TmpfsNode));

    dirDentry->childDentry[dirDentry->childCount++] = targetDentry;

    targetDentry->flag = REGULAR_FILE;
    targetDentry->childCount = 0;
    targetDentry->vnode = *target;
    strcpy(targetDentry->name, component_name);

    targetTmpfsNode->size = 0;
    targetTmpfsNode->dentry = targetDentry;
    (*target)->internal = targetTmpfsNode;

    return 1;
}

int tmpfs_mkdir(VNode *dir_node, VNode **target, const char *component_name) {
    printf("[mkdir] directory name: \"%s\", new folder name: \"%s\"\n", 
        ((TmpfsNode *)(dir_node->internal))->dentry->name, component_name);
    Dentry *dirDentry = ((TmpfsNode *)(dir_node->internal))->dentry;
    Dentry *targetDentry = (Dentry *) malloc(sizeof(Dentry));
    TmpfsNode *targetTmpfsNode = (TmpfsNode *) malloc(sizeof(TmpfsNode));

    dirDentry->childDentry[dirDentry->childCount++] = targetDentry;

    targetDentry->flag = DIRECTORY;
    targetDentry->childCount = 0;
    targetDentry->vnode = *target;
    strcpy(targetDentry->name, component_name);

    targetTmpfsNode->size = 0;
    targetTmpfsNode->dentry = targetDentry;
    (*target)->internal = targetTmpfsNode;

    return 1;
}

int tmpfs_write(File *file, const void *buf, int len) {
    printf("[write] file name: \"%s\"\n", 
        ((TmpfsNode *)(file->vnode->internal))->dentry->name);
    int pos = file->f_pos;
    TmpfsNode *tmpfsNode = file->vnode->internal;
    for (int i = 0; i < len; i++) {
        tmpfsNode->content[i] = ((char *)buf)[pos+i];
        file->f_pos++;
    }
    tmpfsNode->size += len;
    return len;
}

int tmpfs_read(File *file, void *buf, int len) {
    printf("[read] file name: \"%s\"\n", 
        ((TmpfsNode *)(file->vnode->internal))->dentry->name);
    int pos = file->f_pos;
    TmpfsNode *tmpfsNode = file->vnode->internal;
    for (int i = 0; i < len; i++) {
        if (pos + i < tmpfsNode->size) {
            ((char *) buf)[i] = tmpfsNode->content[pos+i];
            file->f_pos++;
        } else {
            return i;
        }
    }
}

int tmpfs_ls(VNode *node) {
    Dentry *dentry = ((TmpfsNode *)(node->internal))->dentry;
    printf("[ls]:\n");
    for (int i = 0; i < dentry->childCount; i++) {
        if (((dentry->childDentry)[i])->flag == REGULAR_FILE) {
            printf(" - (File) %s\n", ((dentry->childDentry)[i])->name);
        } else {
            printf(" - (Folder) %s\n", ((dentry->childDentry)[i])->name);
        }
    }
    return 1;
}