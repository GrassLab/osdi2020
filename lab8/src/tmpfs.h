#ifndef TMPFS
#define TMPFS
#include "vfs.h"
#define TMPFS_FILE_NUM 100
#define TMPFS_DIR_NUM 100

struct tmpfs_file_struct{
    char name[100];
    char *ptr;
    struct tmpfs_dir_struct *dir;
    int valid;
    char content[512];
};

struct tmpfs_dir_struct{
    char name[100];
    char *ptr;
    int file_num;
    struct tmpfs_file_struct* file[TMPFS_FILE_NUM];
    int valid;
};

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);

int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);

struct tmpfs_file_struct file_pool[TMPFS_FILE_NUM] __attribute__((aligned(16u)));

struct tmpfs_dir_struct dir_pool[TMPFS_DIR_NUM] __attribute__((aligned(16u)));

//struct vnode_operations tmpfs_vnode_op = {.lookup = lookup, .create = create};

#endif