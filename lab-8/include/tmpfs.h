#ifndef TMPFS_H
#define TMPFS_H

#include "vfs.h"

#define TMP_FILE_SIZE 512
#define DENTRY_NAME_LEN 128
#define MAX_CHILD_DENTRY 128

typedef struct dentry {
    int flag;
    int childCount;
    char name[DENTRY_NAME_LEN];
    struct vnode *vnode;
    struct dentry *parentDentry;
    struct dentry *childDentry[MAX_CHILD_DENTRY];
} Dentry;

typedef struct tmpfs_node {
    Dentry *dentry;
    int size;
    char content[TMP_FILE_SIZE];
} TmpfsNode;

void dentry_init(Dentry *dentry, char *name, int flag);
int tmpsfs_setup_mount();
void tmpfs_vnode_init(VNode *vode);
int tmpfs_lookup(VNode *dir_node, VNode **target, const char *component_name);
int tmpfs_create(VNode *dir_node, VNode **target, const char *component_name);
int tmpfs_mkdir(VNode *dir_node, VNode **target, const char *component_name);
int tmpfs_write(File *file, const void *buf, int len);
int tmpfs_read(File *file, void *buf, int len);
int tmpfs_ls(VNode *node);

FileSystem *tmpfs;

#endif