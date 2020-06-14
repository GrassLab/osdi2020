
#ifndef FAT32_H
#define FAT32_H

#include "type.h"

typedef struct fat32_partition
{
    uint8_t status_flag;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint8_t starting_sector[4];
    uint8_t number_of_sector[4];
} fat32_partition_t;

void fat32_init ( );

#endif