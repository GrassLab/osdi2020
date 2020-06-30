#ifndef TMPFS_H
#define TMPFS_H
#include "vfs.h"

#define EOF (-1)
#define TMPFS_FILE_SIZE 512

vnode_operations_t* tmpfs_v_ops;
file_operations_t* tmpfs_f_ops;

typedef struct tmpfs_node{
	char buffer[TMPFS_FILE_SIZE];
} tmpfs_node_t;

int tmpfs_setup_mount ( filesystem_t * fs, mount_t * mount );
int tmpfs_lookup ( dentry_t * dir_node, vnode_t ** target, const char * component_name , dentry_t ** dentry);
int tmpfs_create ( dentry_t * dir_node, vnode_t ** target, const char * component_name , dentry_t ** dentry);
int tmpfs_write ( file_t * file, const void * buf, size_t len );
int tmpfs_read ( file_t * file, void * buf, size_t len );
void list_tmpfs(dentry_t* dir);
#endif 
