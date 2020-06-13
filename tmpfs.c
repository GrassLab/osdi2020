#include "vfs.h"
#include "tmpfs.h"
#include "common.h"
#include "printf.h"
#include "string.h"

mount_t tmpfs_mount;
vnode_t tmpfs_root;
vnode_operations_t v_ops;
file_operations_t f_ops;

typedef struct node_t
{
    char data[512];
} node_t;

node_t node_arr[32];

filesystem_t
tmpfs_filesystem()
{
    filesystem_t fs;
    fs.name = "tmpfs";
    fs.setup_mount = tmpfs_setup_mount;
    return fs;
}

int tmpfs_setup_mount(struct filesystem_t *fs, mount_t **mount)
{
    printf("tmpfs setup\n");
    tmpfs_mount.root = &tmpfs_root;
    tmpfs_mount.fs = fs;

    *mount = &tmpfs_mount;

    f_ops.write = tmpfs_write;
    f_ops.read = tmpfs_read;

    v_ops.lookup = tmpfs_lookup;
    v_ops.create = tmpfs_create;

    tmpfs_root.mount = &tmpfs_mount;
    tmpfs_root.v_ops = &v_ops;
    tmpfs_root.f_ops = &f_ops;
    tmpfs_root.internal;

    return 0;
}

typedef struct component_t
{
    char *name;
    file_t *file;
    struct component_t *next;
} component_t;

int tmpfs_write(file_t *file, const void *buf, size_t len) {}

int tmpfs_read(file_t *file, void *buf, size_t len) {}

int tmpfs_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
    printf("tmpfs lookup\n");

    component_t *ptr;
    while (ptr != NULL)
    {
        if (strcmp(ptr->name, component_name) == 0)
        {
            *target = ptr->file->vnode;

            return 0;
        }

        ptr = ptr->next;
    }

    return -1;
    /*
    component_t component;
    node_t *data = &node_arr[0];
    component.file->vnode->internal = data;
    */
}
int tmpfs_create(vnode_t *dir_node, vnode_t **target, const char *component_name) {}