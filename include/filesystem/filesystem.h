#ifndef _FS_H
#define _FS_H

#include "type.h"

#define O_CREAT   00000100

typedef enum
{
    dir = 0,
    file = 1
} vnode_t;

struct vnode 
{
    char name[256];
    struct mount *mount;
    struct vnodeOperations *v_ops;
    struct fileOperations *f_ops;
    vnode_t type;
    void *internal;
};

struct file 
{
    struct vnode *vnode;
    size_t f_pos; // The next read/write position of this file descriptor
    struct fileOperations *f_ops;
    int32_t flags;
};

struct mount 
{
    struct vnode *root;
    struct filesystem *fs;
};

struct filesystem 
{
    char name[256];
    int32_t vnode_token;
    int32_t file_token;
    int32_t dentry_token;
    int32_t fcontent_token;
    int32_t (*setup_mount)(struct filesystem *fs, struct mount *mount);
};

struct fileOperations 
{
    int32_t (*write) (struct file *file, const void *buf, size_t len);
    int32_t (*read) (struct file *file, void *buf, size_t len);
};

struct vnodeOperations 
{
    int32_t (*lookup)(struct vnode *root, struct vnode *target, const char *pathname);
    int32_t (*create)(struct vnode *root, struct vnode *target, const char *pathname);
};

int32_t createFilesystem(const char *fsname);
struct file *vfsOpen(const char* pathname, int flags);
int32_t vfsClose(struct file *file);
int32_t vfsWrite(struct file *file, const void *buf, size_t len) ;
int32_t vfsRead(struct file *file, void *buf, size_t len);

#endif