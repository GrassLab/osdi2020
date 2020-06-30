#ifndef FAT32_H
#define FAT32_H

typedef struct fat32Dentry {
    int flag;
    int childCount;
    // char name[DENTRY_NAME_LEN];
    // struct vnode *vnode;
    // struct dentry *parentDentry;
    // struct dentry *childDentry[MAX_CHILD_DENTRY];
} Fat32Dentry;


void setup_mount();

#endif