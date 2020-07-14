#ifndef _FAT_H
#define _FAT_H

#include "type.h"

struct fatBootFat32 
{
    uint32_t    sectors_per_fat;
    uint16_t    fat_flags;
    uint16_t    version;
    uint32_t    root_cluster;
    uint16_t    fsinfo_sector;
    uint16_t    backup_sector;
    uint8_t     reserved2[12];
    uint8_t     drive_number;
    uint8_t     reserved3;
    uint8_t     ext_sig;
    uint32_t    serial;
    char        label[11];
    char        type[8];
} __attribute__ ((__packed__));

// Boot sector
struct fatBoot 
{
    uint8_t     jmp_boot[3];
    char        oemname[8];
    uint16_t    bytes_per_sector;
    uint8_t     sectors_per_cluster;
    uint16_t    reserved_sectors;
    uint8_t     fat_count;
    uint16_t    root_max_entries;
    uint16_t    total_sectors_small;
    uint8_t     media_info;
    uint16_t    sectors_per_fat_small;
    uint16_t    sectors_per_track;
    uint16_t    head_count;
    uint32_t    fs_offset;
    uint32_t    total_sectors;
    struct fatBootFat32  fat32;
} __attribute__ ((__packed__));

// directory entry structure
struct fatDentry
{
    char      name[8];
    char      ext[3];
    char      attr[9];
    uint16_t  ch;
    uint32_t  attr2;
    uint16_t  cl;
    uint32_t  size;
} __attribute__ ((__packed__));

int32_t fatSetupMount(struct filesystem* fs, struct mount* mount);

#endif