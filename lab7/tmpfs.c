#include "tmpfs.h"
#include "uart.h"
#include "util.h"
#include "vfs.h"

mount_t tmpfs_mount;
vnode_t tmpfs_root;
vnode_operations_t v_ops;
file_operations_t f_ops;
component_t component_table[COMPONENT_TABLE_SIZE];
vnode_t vnode_table[COMPONENT_TABLE_SIZE];
int vnode_table_head = 0;

filesystem_t tmpfs_filesystem()
{
    filesystem_t fs;
    fs.name = "tmpfs";
    fs.setup_mount = tmpfs_setup_mount;

    return fs;
}

int tmpfs_setup_mount(filesystem_t* fs, mount_t** mount)
{
    uart_puts("setup tmpfs\r\n");

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

    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        component_table[i].vnode = 0;
        component_table[i].filename = "";
    }

    return SUCCESS;
}

int tmpfs_write(file_t* file, const void* buf, unsigned long len)
{
    uart_puts("tmpfs_write\r\n");
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        if (component_table[i].vnode == file->vnode) {
            unsigned long f_size = component_table[i].f_size;
            for (int j = 0; j < len; j++) {
                component_table[i].data[file->f_pos++] = ((char*)buf)[j];
            }
            // may encounter some problems
            component_table[i].f_size = file->f_pos;

            component_table[i].data[file->f_pos] = '\0';
            uart_puts(component_table[i].data);
            uart_puts("tmpfs_write success\r\n");
            return SUCCESS;
        }
    }
    uart_puts("tmpfs_write fail\r\n");
    return FAIL;
}

int tmpfs_read(file_t* file, void* buf, unsigned long len)
{
    uart_puts("tmpfs_read\r\n");
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        if (component_table[i].vnode == file->vnode) {
            for (int j = 0; j < component_table[i].f_size; j++) {
                // uart_puts("test\r\n");
                ((char*)buf)[j] = component_table[i].data[j];
            }

            uart_puts(buf);
            uart_puts("tmpfs_read success\r\n");
            return component_table[i].f_size;
        }
    }

    uart_puts("tmpfs_read fail\r\n");
    return FAIL;
}

int tmpfs_lookup(vnode_t* dir_node, vnode_t** target, const char* component_name)
{
    uart_puts("tmpfs_lookup ");
    uart_puts(component_name);
    uart_puts("\r\n");

    component_t* ptr;
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        ptr = &component_table[i];
        if (strcmp((char*)component_name, (char*)ptr->filename)) {
            *target = ptr->vnode;

            uart_puts("tmpfs_lookup success\r\n");
            return SUCCESS;
        }
    }

    uart_puts("tmpfs_lookup fail\r\n");
    return FAIL;
}

int tmpfs_create(vnode_t* dir_node, vnode_t** target, const char* component_name)
{
    uart_puts("tmpfs_create ");
    uart_puts(component_name);
    uart_puts("\r\n");

    component_t* ptr;
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++) {
        if (component_table[i].vnode == 0) {
            ptr = &component_table[i];
            ptr->filename = component_name;
            ptr->f_size = 0;
            ptr->vnode = &vnode_table[vnode_table_head++];
            ptr->vnode->mount = dir_node->mount;
            ptr->vnode->f_ops = &f_ops;
            ptr->vnode->v_ops = &v_ops;
            // internal?

            uart_puts("tmpfs_create success\r\n");
            return SUCCESS;
        }
    }

    uart_puts("tmpfs_create fail\r\n");
    return FAIL;
}