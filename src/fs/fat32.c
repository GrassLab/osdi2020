#include "fs/fat32.h"

#include "allocator.h"
#include "fs/sdhost.h"
#include "uart.h"

fat32_partition_t * sd_root_partition;

void fat32_init ( )
{
    int i;
    uint8_t * sector;

    sd_init ( );

    sector            = (uint8_t *) kmalloc ( sizeof ( uint8_t ) * 512 );
    sd_root_partition = (fat32_partition_t *) kmalloc ( sizeof ( fat32_partition_t ) );

    readblock ( 0, sector );

    for ( i = 0; i < 16; i++ )
    {
        ( (uint8_t *) sd_root_partition )[i] = sector[446 + i];
    }

    kfree ( sector );
}