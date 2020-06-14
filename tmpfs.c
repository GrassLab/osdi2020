#include "vfs.h"
#include "tmpfs.h"
#include "common.h"
#include "printf.h"
#include "string.h"

mount_t tmpfs_mount;
vnode_t tmpfs_root;
vnode_operations_t v_ops;
file_operations_t f_ops;

filesystem_t tmpfs_filesystem()
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
    char data[512];
    size_t f_size;
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
        component_arr[i].vnode = NULL;
        component_arr[i].filename = "";
    }

    return 0;
}

int tmpfs_write(file_t *file, const void *buf, size_t len)
{
    printf("tmpfs write\n");
    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        if (component_arr[i].vnode == file->vnode)
        {
            int pre_f_size = component_arr[i].f_size;
            for (int j = 0; j < len; j++)
            {
                component_arr[i].data[file->f_pos++] = ((char *)buf)[j];
            }
            component_arr[i].f_size = file->f_pos < pre_f_size ? pre_f_size : file->f_pos;

            /*
            component_arr[i].data[file->f_pos] = '\0';
            printf("%s", component_arr[i].data);
            */
            printf("tmpfs write succeeded: file size %d, %x\n", component_arr[i].f_size, component_arr[i].vnode);
            return 0;
        }
    }
    printf("tmpfs write failed\n");
    return -1;
}

int tmpfs_read(file_t *file, void *buf, size_t len)
{
    printf("tmpfs read\n");
    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        if (component_arr[i].vnode == file->vnode)
        {
            len = len < component_arr[i].f_size - file->f_pos ? len : component_arr[i].f_size - file->f_pos;
            for (int j = 0; j < len; j++)
            {
                ((char *)buf)[j] = component_arr[i].data[file->f_pos++];
            }
            printf("tmpfs read succeeded %x\n", component_arr[i].vnode);
            return len;
        }
    }
    printf("tmpfs read failed\n");
    return -1;
}

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

            printf("tmpfs lookup find: %s, %x\n", component_name, ptr->vnode);
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
            component_arr[i].filename = (char *)component_name;
            component_arr[i].f_size = 0;
            component_arr[i].vnode = &vnode_arr[vnode_arr_head++];
            component_arr[i].vnode->mount = dir_node->mount;
            component_arr[i].vnode->f_ops = &f_ops;
            component_arr[i].vnode->v_ops = &v_ops;
            component_arr[i].vnode->internal;

            *target = component_arr[i].vnode;

            printf("tmpfs create succeeded: %s, %x\n", component_name, *target);
            return 0;
        }
    }
    printf("tmpfs create fail: %s\n", component_name);
    return -1;
}
