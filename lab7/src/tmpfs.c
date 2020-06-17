#include "tmpfs.h"
filesystem_t file_sys;
vnode_t vnode;
vnode_op_t v_ops;
file_op_t f_ops;
filesystem_t* fs;


void tmpfs_init()
{
    fs = file_sys;
    fs->name = "tmpfs";
    fs->setup_mount = tmpfs_setup_mount;
    register_filesystem(fs);
}

int tmpfs_setup_mount(filesystem_t* fs, mount_t* mount)
{
    mount->fs = fs;
    mount->root = vnode;

    mount->root->mount = mount;
    mount->root->v_ops = v_ops;
    mount->root->v_ops->lookup = tmpfs_lookup;
    mount->root->v_ops->create = tmpfs_create;
    mount->root->f_ops = f_ops;
    mount->root->f_ops->write = tmpfs_write;
    mount->root->f_ops->read = tmpfs_read;
    return 0;
}

int tmpfs_lookup(vnode_t * dir_node, vnode_t ** target, const char * component_name)
{
}

int tmpfs_create(vnode_t * dir_node, vnode_t ** target, const char * component_name)
{
}

int tmpfs_write(file_t * file, const void * buf, size_t len)
{
}

int tmpfs_read(file_t * file, void * buf, size_t len)
{
} 
