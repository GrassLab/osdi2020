#include "vfs.h"
#include "fat32.h"
#include "common.h"
#include "printf.h"

mount_t fat32_mount;
vnode_t fat32_root;
vnode_operations_t v_ops;
file_operations_t f_ops;

filesystem_t fat32_filesystem()
{
    filesystem_t fs;
    fs.name = "fat32";
    fs.setup_mount = fat32_setup_mount;
    return fs;
}

int fat32_setup_mount(struct filesystem_t *fs, mount_t **mount)
{
    printf("fat32 setup\n");
    fat32_mount.root = &fat32_root;
    fat32_mount.fs = fs;

    *mount = &fat32_mount;

    f_ops.write = fat32_write;
    f_ops.read = fat32_read;

    v_ops.lookup = fat32_lookup;
    v_ops.create = fat32_create;

    fat32_root.mount = &fat32_mount;
    fat32_root.v_ops = &v_ops;
    fat32_root.f_ops = &f_ops;
    fat32_root.internal;

    return 0;
}

int fat32_write(file_t *file, const void *buf, size_t len) {}

int fat32_read(file_t *file, void *buf, size_t len) {}

int fat32_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
    printf("fat32 lookup\n");
}
int fat32_create(vnode_t *dir_node, vnode_t **target, const char *component_name) {}