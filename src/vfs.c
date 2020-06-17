#include "vfs.h"

#include "allocator.h"
#include "string.h"
#include "uart.h"

// global variable used in this file
mount_t * rootfs;

// function used in this file only.
int parsing_last_component ( dentry_t ** dentry, char ** component_name, const char * pathname );

int register_filesystem ( file_sys_t * fs )
{
    if ( !strcmp ( fs->name, "fat32" ) )
    {
        rootfs = (mount_t *) kmalloc ( sizeof ( mount_t ) );

        fs->setup_mount ( fs, rootfs );

        return 1;
    }
    // other type of file system would be put on another global variable

    return 0;
}

file_t * vfs_open ( const char * pathname, file_op_flag_t flags )
{
    dentry_t * dentry;
    dentry_t * new_d;
    file_t * file;

    char * component_name;
    int res;

    res = parsing_last_component ( &dentry, &component_name, pathname );

    if ( res == -1 )
    {
        uart_printf ( "dir not found.\n" );
        return NULL;
    }

    // create and open file
    if ( flags & O_CREAT )
    {
        res = dentry->vnode->v_ops->create ( dentry, &new_d, component_name );

        if ( res == -1 )
        {
            uart_printf ( "dir is exist.\n" );
        }
        else if ( res == -2 )
        {
            uart_printf ( "MAX child dir is exceed.\n" );
        }

        file         = (file_t *) kmalloc ( sizeof ( file_t ) );
        file->dentry = new_d;
        file->f_pos  = 0;

        return file;
    }
    // just open a file
    else
    {
        res = dentry->vnode->v_ops->lookup ( dentry, &new_d, component_name );

        if ( res == -1 )
        {
            uart_printf ( "%s: file not found.\n", pathname );
            return NULL;
        }

        file         = (file_t *) kmalloc ( sizeof ( file_t ) );
        file->dentry = new_d;
        file->f_pos  = 0;

        return file;
    }
}
int vfs_close ( file_t * file )
{
    kfree ( file );
    return 0;
}

int vfs_write ( file_t * file, const void * buf, size_t len )
{
    int res;
    res = file->dentry->vnode->f_ops->write ( file, buf, len );

    if ( res == -1 )
        uart_printf ( "Exceed MAX len of the file." );

    return res;
}

int vfs_read ( struct file * file, void * buf, size_t len )
{
    return file->dentry->vnode->f_ops->read ( file, buf, len );
}

int parsing_last_component ( dentry_t ** dentry, char ** component_name, const char * pathname )
{
    int path_len        = strlen ( pathname );
    int path_name_count = 0;
    int component_name_start_index;
    int len;

    char * temp_component_name;
    int look_up_res;

    dentry_t * current_dentry = rootfs->root_dir;
    dentry_t * temp_dentry;

    component_name_start_index = 0;

    while ( pathname[path_name_count] != '\0' )
    {
        if ( pathname[path_name_count] != '/' )
        {
            // this is last component
            if ( path_name_count == path_len - 1 )
            {
                len                 = path_name_count - component_name_start_index + 2;
                temp_component_name = (char *) kmalloc ( sizeof ( char ) * len );
                strncpy ( temp_component_name, pathname + component_name_start_index, len - 1 );

                temp_component_name[len - 1] = '\0';

                *component_name = temp_component_name;
                *dentry         = current_dentry;

                return 0;
            }
            // just go through the component name
            else
            {
                path_name_count++;
                continue;
            }
        }
        // a break point of the path name
        else
        {
            len                 = path_name_count - component_name_start_index + 2;
            temp_component_name = (char *) kmalloc ( sizeof ( char ) * len );
            strncpy ( temp_component_name, pathname + component_name_start_index, len - 1 );

            temp_component_name[len - 1] = '\0';

            look_up_res = current_dentry->vnode->v_ops->lookup ( current_dentry, &temp_dentry, temp_component_name );

            // not found in the dir
            if ( look_up_res == -1 )
            {
                // it is the last component of the path, and it might be a new dir or a new file or just a file, fo it can be fount in the "lookup" func.
                if ( path_name_count == path_len )
                {
                    *component_name = temp_component_name;
                    *dentry         = current_dentry;

                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            // check if there is another component after this
            else if ( path_name_count == path_len || ( path_name_count + 1 == path_len && pathname[path_name_count + 1] == '/' ) )
            {
                *component_name = temp_component_name;
                *dentry         = current_dentry;

                return 0;
            }
            // there is still another copmonent after this
            else
            {
                current_dentry = temp_dentry;
                kfree ( temp_component_name );
                path_name_count++;
                component_name_start_index = path_name_count;
            }
        }
    }

    return -1;
}