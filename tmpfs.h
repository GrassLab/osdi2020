#ifndef _TMPFS_H_
#define _TMPFS_H_

#include "vfs.h"

filesystem_t tmpfs_filesystem();

int tmpfs_setup_mount(struct filesystem_t *fs, mount_t **mount);
int tmpfs_write(file_t *file, const void *buf, size_t len);
int tmpfs_read(file_t *file, void *buf, size_t len);

int tmpfs_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name);
int tmpfs_create(vnode_t *dir_node, vnode_t **target, const char *component_name);

#endif