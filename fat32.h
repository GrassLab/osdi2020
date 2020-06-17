#ifndef _FAT32_H_
#define _FAT32_H_

#include "vfs.h"

filesystem_t fat32_filesystem();

int fat32_setup_mount(struct filesystem_t *fs, mount_t **mount);
int fat32_write(file_t *file, const void *buf, size_t len);
int fat32_read(file_t *file, void *buf, size_t len);

int fat32_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name);
int fat32_create(vnode_t *dir_node, vnode_t **target, const char *component_name);

#endif