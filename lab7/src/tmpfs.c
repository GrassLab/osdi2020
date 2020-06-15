#include "tmpfs.h"

mount tmpfs_mount;
vnode tmpfs_root;
file_operations f_ops;
vnode_operations v_ops;

filesystem tmpfs_filesystem(){
    filesystem fs;
    fs.name = "tmpfs";
    fs.setup_mount = tmpfs_setup_mount;
    return fs;
}

typedef struct component_t
{
    char *filename;
    vnode *vnode;
    char data[512];
    size_t f_size;
} component;

#define COMPONENT_ARR_SIZE 16
component component_arr[COMPONENT_ARR_SIZE];

vnode vnode_arr[COMPONENT_ARR_SIZE];
int vnode_arr_head = 0;

int tmpfs_setup_mount(struct filesystem_t *fs, struct mount_t **mount)
{
    uart_puts("tmpfs setup\n");
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

    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        component_arr[i].vnode = NULL;
        component_arr[i].filename = "";
    }
    return 0;
}

int tmpfs_lookup(vnode *dir_node, vnode **target, const char *component_name)
{
    uart_puts("tmpfs lookup ");
    uart_puts(component_name);
    uart_puts("\n");

    component *ptr;
    for (int i = 0; i < COMPONENT_ARR_SIZE; i++)
    {
        ptr = &component_arr[i];
        if (my_strcmp(ptr->filename, component_name) == 1)
        {
            *target = ptr->vnode;

            uart_puts("tmpfs lookup find: ");
            uart_puts(component_name);
            uart_puts(" ");
            uart_hex(ptr->vnode);
            uart_puts("\n");
            return 0;
        }
    }
    uart_puts("tmpfs lookup not find: ");
    uart_puts(component_name);
    uart_puts("\n");
    return -1;
    
}
int tmpfs_create(vnode *dir_node, vnode **target, const char *component_name) {
    uart_puts("tmpfs create: ");
    uart_puts(component_name);
    uart_puts("\n");
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

            uart_puts("tmpfs create succeeded: ");
            uart_puts(component_name);
            uart_puts(" ");
            uart_hex(*target);
            uart_puts("\n");
            return 0;
        }
    }
    uart_puts("tmpfs create fail: ");
    uart_puts(component_name);
    uart_puts("\n");
    return -1;
}

int tmpfs_write(file *file, const void *buf, size_t len) {
    uart_puts("tmpfs write\n");

}

int tmpfs_read(file *file, void *buf, size_t len) {}
