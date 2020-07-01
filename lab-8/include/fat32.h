#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"

typedef struct fat32Dentry {
    char name[8];			 // 0x00
    char ext[3];		     // 0x08
    char ignore1[9];		 // 0x0b
    unsigned short startHi;  // 0x14
    char ignore2[4];		 // 0x16
    unsigned short startLo;  // 0x1a
    unsigned int size;		 // 0x1c
} __attribute__ ((packed)) Fat32Dentry;

typedef struct boot_sector
{
    char ignore1[0xe];
    unsigned short reservedCount;	//0xe
    unsigned char fatNum;	// 0x10
    char ignore2[0x13];
    unsigned int sectorsPerFat;	// 0x24
    char ignore3[4];
    unsigned int clusterNumOfRoot;	// 0x2c
} __attribute__ ((packed)) BootSector ;

typedef struct partition_entry {
    char ignore[0x1c6];
    unsigned int lba;
    unsigned int size;
} __attribute__ ((packed)) PartitionEntry ;

typedef struct fat32_info {
    unsigned int lba;
    unsigned int size;
    unsigned short reservedCount;
    unsigned char fatNum;
    unsigned int sectorsPerFat;
    unsigned int clusterNumOfRoot;
} Fat32Info;

typedef struct fat32Node {
    Fat32Info info;
    unsigned int dentry;
    unsigned int clusterIndex;
    unsigned int dirIndex;
} Fat32Node;

FileSystem *fat32fs;

void fat32_setup_mount(FileSystem *fs, Mount *mount);
Fat32Dentry *find_fat32_dentry(Fat32Node *node);
int set_fat32_dentry(Fat32Node *node, Fat32Dentry *dentry);
int fat32_write(File *file, const void *buf, int len);
int fat32_read(File *file, void *buf, int len);
int fat32_ls(VNode *node);
int fat32_lookup(VNode *dir_node, VNode **target, const char *component_name);
int fat32_create(VNode *dir_node, VNode **target, const char *component_name);
int fat32_mkdir(VNode *dir_node, VNode **target, const char *component_name);

#endif