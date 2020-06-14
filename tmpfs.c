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

typedef struct component_t
{
    char *filename;
    vnode_t *vnode;
    ;
    struct component_t *next;
} component_t;

#define COMPONENT_ARR_SIZE 16
component_t component_arr[COMPONENT_ARR_SIZE];

vnode_t vnode_arr[COMPONENT_ARR_SIZE];
int vnode_arr_head = 0;

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

    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        component_arr[i].next = NULL;
        component_arr[i].vnode = NULL;
        component_arr[i].filename = "";
    }

    return 0;
}

int tmpfs_write(file_t *file, const void *buf, size_t len) {}

int tmpfs_read(file_t *file, void *buf, size_t len) {}

int tmpfs_lookup(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
    printf("tmpfs lookup: %s\n", component_name);

    component_t *ptr;
    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        ptr = &component_arr[i];
        if (strcmp(ptr->filename, component_name) == 0)
        {
            *target = ptr->vnode;

            printf("tmpfs lookup find: %s\n", component_name);
            return 0;
        }
    }
    printf("tmpfs lookup not find: %s\n", component_name);

    return -1;
    /*
    component_t component;
    node_t *data = &node_arr[0];
    component.file->vnode->internal = data;
    */
}
int tmpfs_create(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
    printf("tmpfs create: %s\n", component_name);
    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        if (component_arr[i].vnode == NULL)
        {
            component_arr[i].filename = component_name;
            component_arr[i].vnode = &vnode_arr[vnode_arr_head++];

            printf("tmpfs create succeeded: %s\n", component_name);
            return 0;
        }
    }
    printf("tmpfs create fail: %s\n", component_name);
    return -1;
}
