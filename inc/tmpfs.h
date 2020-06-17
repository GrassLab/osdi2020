#ifndef TMPFS_H
#define TMPFS_H
#define DIR_MAX 10
#define TMP_BUF_SIZE 512
#include "vfs.h"

typedef enum {
    FILE_TYPE_F,
    FILE_TYPE_D,
    FILE_TYPE_N
} FILE_TYPE;

struct tmpfs_buf{
	int flag;
    size_t size;
	char buffer[TMP_BUF_SIZE];
};

struct fentry {
    char name[10];
    FILE_TYPE type;
    struct vnode* vnode;
    struct fentry* list[DIR_MAX];
    struct fentry* parent;
    struct tmpfs_buf* buf;
};

extern struct file_operations* tmpfs_f_ops;
extern struct vnode_operations* tmpfs_v_ops;

void init_dentry(struct fentry* fentry, struct vnode* vnode, char* name);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_chdir(struct vnode* dir_node, struct vnode** target,
                const char* component_name);
int tmpfs_mount(struct filesystem* fs, struct mount* mount);

int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_read(struct file* file, void* buf, size_t len);
int tmpfs_mkdir(struct vnode* dir_node, const char* component_name);
int tmpfs_list(struct file* file);
#endif
