#include "tmpfs.h"

#include "allocator.h"
#include "vfs.h"

// function that would be used in this file only.
int tmpfs_setup_mount ( file_sys_t * fs, mount_t * mount );
int tmpfs_lookup ( vnode_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_create ( vnode_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_write ( file_t * file, const void * buf, size_t len );
int tmpfs_read ( file_t * file, void * buf, size_t len );

void tmpfs_init ( )
{
    file_sys_t * fs = (file_sys_t *) kmalloc ( sizeof ( file_sys_t ) );
    fs->name        = "tmpfs";
    fs->setup_mount = tmpfs_setup_mount;

    register_filesystem ( fs );
}

int tmpfs_setup_mount ( file_sys_t * fs, mount_t * mount )
{
    mount->fs   = fs;
    mount->root = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );

    mount->root->mount         = mount;
    mount->root->v_ops         = (vnode_op_t *) kmalloc ( sizeof ( vnode_op_t ) );
    mount->root->v_ops->lookup = tmpfs_lookup;
    mount->root->v_ops->create = tmpfs_create;
    mount->root->f_ops         = (file_op_t *) kmalloc ( sizeof ( file_op_t ) );
    mount->root->f_ops->write  = tmpfs_write;
    mount->root->f_ops->read   = tmpfs_read;

    return 0;
}

// int tmpfs_lookup ( vnode_t * dir_node, vnode_t ** target, const char * component_name )
// {
// }
//
// int tmpfs_create ( vnode_t * dir_node, vnode_t ** target, const char * component_name )
// {
// }
//
// int tmpfs_write ( file_t * file, const void * buf, size_t len )
// {
// }
//
// int tmpfs_read ( file_t * file, void * buf, size_t len )
// {
// }