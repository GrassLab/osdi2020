#ifndef _TMPFS_H_
#define _TMPFS_H_

#include "tools.h"

struct filesystem_t tmpfs_filesystem();

int tmpfs_setup_mount(struct filesystem_t *fs, struct mount_t **mount);
int tmpfs_write(struct file_t *file, const void *buf, size_t len);
int tmpfs_read(struct file_t *file, void *buf, size_t len);

int tmpfs_lookup(struct vnode_t *dir_node, struct vnode_t  **target, const char *component_name);
int tmpfs_create(struct vnode_t *dir_node, struct vnode_t  **target, const char *component_name);

#endif 