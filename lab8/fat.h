#ifndef _FAT32_H_
#define _FAT32_H_

#include "sd.h"
#include "uart.h"
#include "vfs.h"

unsigned int fat32_root;

// // the BIOS Parameter Block (in Volume Boot Record)
// typedef struct mbr_partition_t
// {
//     char _[0x1c6];
//     unsigned int lba;
//     unsigned int size;
// } mbr_partition_t;

// struct boot_sector
// {
//     char ignore1[0xe];
//     unsigned short count_of_reserved; // 0x00E
//     unsigned char num_of_fat;         // 0x010
//     char ignore2[0x13];
//     unsigned int sectors_per_fat; // 0x024
//     char ignore3[4];
//     unsigned int cluster_num_of_root; // 0x02c
// } __attribute__((packed));

// typedef struct fat32_info_t
// {
//     unsigned int lba;
//     unsigned int size;
//     unsigned short count_of_reserved;
//     unsigned char num_of_fat;
//     unsigned int sectors_per_fat;
//     unsigned int cluster_num_of_root;
//     unsigned int offset;
// } fat32_info_t;

typedef struct file_info_t {
    char name[8]; // 0x00
    char extension[3]; // 0x08
    char ignore1[9]; // 0x0b
    unsigned short start_hi; // 0x14
    char ignore2[4]; // 0x16
    unsigned short start_lo; // 0x1a
    unsigned int size; // 0x1c
} file_info_t;

int fat_getpartition(void);
int fat32_lookup(unsigned int dir_cluster, const char* filename, file_info_t* r_file_info);
void fat32_readfile(unsigned int dir_cluster, const char* filename);
int fat32_read(unsigned int dir_cluster, const char* filename, char* r_buf, int start, int len);
int fat32_write(unsigned int dir_cluster, const char* filename, char* w_buf, int start, int len);
int fat32_mount();

#endif