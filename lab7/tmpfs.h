#ifndef _TMPFS_H
#define _TMPFS_H

#include "vfs.h"

#define COMPONENT_TABLE_SIZE 16

typedef struct component {
    char* filename;
    vnode_t* vnode;
    char data[512];
    unsigned long f_size;
} component_t;

filesystem_t tmpfs_filesystem();
int tmpfs_setup_mount(filesystem_t* fs, mount_t** mount);
int tmpfs_write(file_t* file, const void* buf, unsigned long len);
int tmpfs_read(file_t* file, void* buf, unsigned long len);
int tmpfs_lookup(vnode_t* dir_node, vnode_t** target, const char* component_name);
int tmpfs_create(vnode_t* dir_node, vnode_t** target, const char* component_naem);

#endif