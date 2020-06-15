#include "fs/tmpfs.h"

#include "allocator.h"
#include "string.h"
#include "type.h"
#include "uart.h"
#include "vfs.h"

// function that would be used in this file only.
int tmpfs_setup_mount ( file_sys_t * fs, mount_t * mount );
void tmpfs_setup_vnode ( vnode_t * vnode, mount_t * mount );
int tmpfs_lookup ( dentry_t * dir_node, dentry_t ** target, const char * component_name );
int tmpfs_create ( dentry_t * dir_node, dentry_t ** target, const char * component_name );
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
    mount->fs = fs;

    mount->root = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
    tmpfs_setup_vnode ( mount->root, mount );

    // set up root dir
    mount->root_dir        = (dentry_t *) kmalloc ( sizeof ( dentry_t ) );
    mount->root_dir->vnode = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
    tmpfs_setup_vnode ( mount->root_dir->vnode, mount );
    mount->root_dir->flag          = DIRECTORY;
    mount->root_dir->child_amount  = 0;
    mount->root_dir->parent_dentry = NULL;

    mount->root_dir->internal                                = (tmpfs_node_t *) kmalloc ( sizeof ( tmpfs_node_t ) );
    ( (tmpfs_node_t *) ( mount->root_dir->internal ) )->name = "/";

    return 0;
}

void tmpfs_setup_vnode ( vnode_t * vnode, mount_t * mount )
{
    vnode->v_ops = (vnode_op_t *) kmalloc ( sizeof ( vnode_op_t ) );
    vnode->f_ops = (file_op_t *) kmalloc ( sizeof ( file_op_t ) );
    vnode->mount = mount;

    vnode->v_ops->lookup = tmpfs_lookup;
    vnode->v_ops->create = tmpfs_create;
    vnode->f_ops->write  = tmpfs_write;
    vnode->f_ops->read   = tmpfs_read;
}

int tmpfs_lookup ( dentry_t * dir, dentry_t ** target, const char * component_name )
{
    int i;

    for ( i = 0; i < dir->child_amount; i++ )
    {
        if ( strcmp ( ( (tmpfs_node_t *) ( dir->child_dentry[i]->internal ) )->name, component_name ) == 0 )
        {
            ( *target ) = dir->child_dentry[i];
            return 1;
        }
    }

    *target = NULL;
    return -1;
}

int tmpfs_create ( dentry_t * dir_node, dentry_t ** target, const char * component_name )
{
    int i;
    dentry_t * new_d;
    tmpfs_node_t * node;

    if ( dir_node->child_amount >= MAX_CHILD_DIR )
    {
        return -2;
    }

    // check if duplicate
    for ( i = 0; i < dir_node->child_amount; i++ )
    {
        if ( strcmp ( ( (tmpfs_node_t *) ( dir_node->child_dentry[i]->internal ) )->name, component_name ) == 0 )
        {
            return -1;
        }
    }

    new_d = (dentry_t *) kmalloc ( sizeof ( dentry_t ) );

    new_d->flag  = FILE;
    new_d->vnode = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
    tmpfs_setup_vnode ( new_d->vnode, NULL );

    new_d->parent_dentry = dir_node;

    node              = (tmpfs_node_t *) kmalloc ( sizeof ( tmpfs_node_t ) );
    node->file_length = 0;
    node->name        = (char *) kmalloc ( sizeof ( char ) * ( strlen ( component_name ) + 1 ) );
    strcpy ( node->name, component_name );
    new_d->internal = node;

    dir_node->child_dentry[dir_node->child_amount] = new_d;
    ( dir_node->child_amount )++;

    *target = new_d;

    return 0;
}

int tmpfs_write ( file_t * file, const void * buf, size_t len )
{
    if ( len + file->f_pos > TMPFS_FIEL_BUFFER_MAX_LEN )
        return -1;

    strncpy ( ( (tmpfs_node_t *) ( file->dentry->internal ) )->buffer + file->f_pos, buf, len );

    ( file->f_pos ) += len;
    ( ( (tmpfs_node_t *) ( file->dentry->internal ) )->file_length ) += len;

    return 0;
}

int tmpfs_read ( file_t * file, void * buf, size_t len )
{
    int readlen;
    size_t filelen = ( ( (tmpfs_node_t *) ( file->dentry->internal ) )->file_length );

    readlen = filelen > len ? len : filelen;

    strncpy ( buf, ( (tmpfs_node_t *) ( file->dentry->internal ) )->buffer, readlen );

    return readlen;
}