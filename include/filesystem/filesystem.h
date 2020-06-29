#ifndef _FS_H
#define _FS_H

#include "type.h"

#define O_CREAT   00000100

#define NAME_BUFFER_SIZE 256
#define MAX_DIR_CHILD 100

struct dentry
{
    struct vnode* vnode;
    struct vnode** child;
    size_t child_num;
};

struct fcontent
{
    char *content;
    size_t fsize;
};

typedef enum
{
    dir = 0,
    file
} vnode_t;

struct vnode 
{
    char *name;
    struct mount *mount;
    struct vnodeOperations *v_ops;
    struct fileOperations *f_ops;
    vnode_t type;
    void *internal;
    void *node_info;
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
    char *name;
    int32_t vnode_token;
    int32_t file_token;
    int32_t dentry_token;
    int32_t fcontent_token;
    int32_t name_token;
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
    int32_t (*create)(struct vnode *root, struct vnode *target, const char *pathname, vnode_t type);
    int32_t (*listdir)(struct vnode *root, const char *pathname);
};

int32_t createFilesystem(const char *fsname);
struct file *vfsOpen(const char* pathname, int flags);
int32_t vfsClose(struct file *file);
int32_t vfsWrite(struct file *file, const void *buf, size_t len) ;
int32_t vfsRead(struct file *file, void *buf, size_t len);
int32_t mkdir(const char* pathname);
int32_t lsdir(const char* pathname);

#endif