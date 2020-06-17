#ifndef TMPFS_H
#define TMPFS_H
#include "vfs.h"
void tmpfs_init();

int tmpfs_setup_mount ( filesystem_t * fs, mount_t * mount );
int tmpfs_lookup ( vnode_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_create ( vnode_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_write ( file_t * file, const void * buf, size_t len );
int tmpfs_read ( file_t * file, void * buf, size_t len );
#endif 
