#include "fs/fat32.h"

#include "allocator.h"
#include "fs/sdhost.h"
#include "string.h"
#include "uart.h"
#include "vfs.h"

// global variable used in this file
fat32_partition_t * sd_root_partition;
fat32_boot_sector_t * sd_root_bst;

// funciton used in this file only.
int fat32_setup_mount ( file_sys_t * fs, mount_t * mount );
void fat32_setup_vnode ( vnode_t * vnode, mount_t * mount );
int fat32_lookup ( dentry_t * dir_node, dentry_t ** target, const char * component_name );
// int fat32_write ( file_t * file, const void * buf, size_t len );
// int fat32_read ( file_t * file, void * buf, size_t len );
void fat32_print_info ( );

#define CONCATE_8( a, b ) ( ( a ) | ( ( ( uint16_t ) ( b ) ) << 8 ) )

void fat32_init ( )
{
    int i;
    uint8_t * sector;
    // uint16_t tmp_1, tmp_2;
    // uint32_t tmp_3;
    file_sys_t * fs;

    sd_init ( );

    sector            = (uint8_t *) kmalloc ( sizeof ( uint8_t ) * 512 );
    sd_root_partition = (fat32_partition_t *) kmalloc ( sizeof ( fat32_partition_t ) );

    readblock ( 0, sector );

    for ( i = 0; i < 16; i++ )
        ( (uint8_t *) sd_root_partition )[i] = sector[446 + i];

    // read first boot sector
    readblock ( sd_root_partition->starting_sector, sector );

    sd_root_bst = (fat32_boot_sector_t *) sector;

    sd_root_bst->bytes_per_logic_sector = CONCATE_8 ( sector[11], sector[12] );
    sd_root_bst->n_root_dir_entries     = CONCATE_8 ( sector[17], sector[18] );

    sd_root_partition->root_sector_abs = ( sd_root_bst->n_sector_per_fat_32 * sd_root_bst->n_file_alloc_tabs ) + sd_root_bst->n_reserved_sectors;

    fs              = (file_sys_t *) kmalloc ( sizeof ( file_sys_t ) );
    fs->name        = "fat32";
    fs->setup_mount = fat32_setup_mount;

    register_filesystem ( fs );
}

int fat32_setup_mount ( file_sys_t * fs, mount_t * mount )
{
    int i;
    dentry_t * root_dir;
    uint8_t * sector;
    fat32_dir_t * dir;
    fat32_node_t * node;
    dentry_t * child;

    mount->fs = fs;

    mount->root = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
    fat32_setup_vnode ( mount->root, mount );

    // set up root dir
    root_dir        = (dentry_t *) kmalloc ( sizeof ( dentry_t ) );
    mount->root_dir = root_dir;
    root_dir->vnode = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
    fat32_setup_vnode ( root_dir->vnode, mount );

    root_dir->flag          = DIRECTORY;
    root_dir->child_amount  = 0;
    root_dir->parent_dentry = NULL;

    node               = (fat32_node_t *) kmalloc ( sizeof ( fat32_node_t ) );
    node->name[0]      = '/';
    node->name[1]      = '\0';
    root_dir->internal = (void *) node;

    // appned child
    sector = (uint8_t *) kmalloc ( sizeof ( uint8_t ) * 512 );
    dir    = (fat32_dir_t *) sector;

    readblock ( sd_root_partition->root_sector_abs + sd_root_partition->starting_sector, sector );

    for ( i = 0; dir[i].name[0] != '\0'; i++ )
    {
        child = (dentry_t *) kmalloc ( sizeof ( dentry_t ) );
        node  = (fat32_node_t *) kmalloc ( sizeof ( fat32_node_t ) );

        // update root
        root_dir->child_dentry[root_dir->child_amount] = child;
        root_dir->child_amount++;

        // update child
        child->flag  = dir[i].attr[0] & 16 ? DIRECTORY : FILE;
        child->vnode = (vnode_t *) kmalloc ( sizeof ( vnode_t ) );
        fat32_setup_vnode ( child->vnode, NULL );
        child->child_amount  = 0;
        child->parent_dentry = root_dir;
        child->internal      = (void *) node;

        node->cluster = ( ( ( uint32_t ) ( dir[i].cluster_high ) ) << 16 ) | ( dir[i].cluster_low );
        node->size    = dir[i].size;
        strncpy ( node->name, dir[i].name, 8 );
        node->name[8] = '\0';
        strncpy ( node->ext, dir[i].ext, 3 );
        node->ext[3] = '\0';
    }

    kfree ( sector );

    return 0;
}

void fat32_setup_vnode ( vnode_t * vnode, mount_t * mount )
{
    vnode->v_ops = (vnode_op_t *) kmalloc ( sizeof ( vnode_op_t ) );
    vnode->f_ops = (file_op_t *) kmalloc ( sizeof ( file_op_t ) );
    vnode->mount = mount;

    vnode->v_ops->lookup = fat32_lookup;
    vnode->v_ops->create = NULL;
    vnode->f_ops->write  = NULL;
    vnode->f_ops->read   = NULL;
}

int fat32_lookup ( dentry_t * dir_node, dentry_t ** target, const char * component_name )
{
    fat32_node_t * node;
    int i, j;
    char * dot;
    char * cpy_name;
    char cmp_name[9];
    char cmp_ext[4];

    cpy_name = (char *) kmalloc ( sizeof ( char ) * strlen ( component_name ) );
    strcpy ( cpy_name, component_name );

    dot = strchr ( cpy_name, '.' );

    if ( dot != NULL )
        *dot = '\0';

    // pad name with space
    strcpy ( cmp_name, cpy_name );
    for ( i = strlen ( cmp_name ); i < 8; i++ )
        cmp_name[i] = ' ';
    cmp_name[i] = '\0';

    for ( i = 0; i < dir_node->child_amount; i++ )
    {
        node = (fat32_node_t *) ( dir_node->child_dentry[i]->internal );

        // check file name is the same
        if ( !strcasecmp ( cmp_name, node->name ) )
        {
            // check file ext is the same
            if ( dot != NULL )
                dot += 1;
            else
                dot = cpy_name + strlen ( cpy_name );

            // pad with space
            strcpy ( cmp_ext, dot );
            for ( j = strlen ( cmp_ext ); j < 3; j++ )
                cmp_ext[j] = ' ';
            cmp_ext[j] = '\0';

            if ( !strcasecmp ( cmp_ext, node->ext ) )
            {
                *target = dir_node->child_dentry[i];
                kfree ( cpy_name );
                return 1;
            }
        }
    }

    kfree ( cpy_name );

    *target = NULL;

    return -1;
}

// int fat32_write ( file_t * file, const void * buf, size_t len )
// {
// }

// int fat32_read ( file_t * file, void * buf, size_t len )
// {
// }

void fat32_print_info ( )
{
    uart_printf ( "[FAT] bytes per sectors: %d\n", sd_root_bst->bytes_per_logic_sector );
    uart_printf ( "[FAT] sector per clusters: %d\n", sd_root_bst->logic_sector_per_cluster );
    uart_printf ( "[FAT] number of fat: %d\n", sd_root_bst->n_file_alloc_tabs );
    uart_printf ( "[FAT] number of reserved sector: %d\n", sd_root_bst->n_reserved_sectors );

    uint32_t root_sec = ( sd_root_bst->n_sector_per_fat_32 * sd_root_bst->n_file_alloc_tabs ) + sd_root_bst->n_reserved_sectors;
    uint8_t * sector  = (uint8_t *) kmalloc ( sizeof ( uint8_t ) * 512 );
    fat32_dir_t * dir = (fat32_dir_t *) sector;
    int i;

    readblock ( root_sec + sd_root_partition->starting_sector, sector );

    for ( i = 0; dir[i].name[0] != '\0'; i++ )
    {
        uart_puts ( dir[i].name );
        uart_puts ( "\n" );
    }

    kfree ( sector );
}