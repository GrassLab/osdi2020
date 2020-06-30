#ifndef _FAT32_H_
#define _FAT32_H_

#include "fat32.h"
#include "sdhost.h"
#include "printf.h"
#include "string.h"

extern unsigned int fat32_root;

void print_block(int block)
{
    char buf[512];
    readblock(block, &buf);

    for (int i = 0; i < 512; i += 16)
    {
        uart_send_hex(i + block * 512);
        uart_puts(": ");
        for (int j = 0; j < 16; j += 2)
        {
            printf("%02x", (buf[i + j]));
            printf("%02x ", (buf[i + j + 1]));
        }
        for (int j = 0; j < 16; j++)
        {
            switch (buf[i + j])
            {
            case '\n':
                printf("\\n");
                break;
            case '\r':
                printf("\\r");
                break;
            case 0:
                printf(".");
                break;
            default:
                if (buf[i + j] < 32)
                    printf(" ");
                else
                    printf("%c", (buf[i + j]));
            }
        }
        printf("\n\r");
    }
    printf("===\n\r");
}

// the BIOS Parameter Block (in Volume Boot Record)
typedef struct
{
    // s: sector
    // c: cluster
    char _[0x00B];
    unsigned short bps;   // 0x00B
    char spc;             // 0x00D
    unsigned short rsc;   // 0x00E
    char nfat;            // 0x010
    unsigned short nroot; // 0x011
    char media;           // 0x015
    unsigned short spf16; // 0x016
    unsigned short spt;   // 0x018 sector per
    unsigned short _0;    // 0x01A
    unsigned int hs;
    unsigned int ts32;
    unsigned int spf32;
    unsigned short flg;
    unsigned short version;
    unsigned int root_start_cluster;
    unsigned short num_boot_sector;
    unsigned short pos_reversed_boot;

    //char _1[29];
    char fst[8];
} __attribute__((packed)) bpb_t;

typedef struct mbr_partition_t
{
    char _[0x1c6];
    unsigned int lba;
    unsigned int size;
} __attribute__((packed)) mbr_partition_t;

/**
 * Get the starting LBA address of the first partition
 * so that we know where our FAT file system starts, and
 * read that volume's BIOS Parameter Block
 */
typedef struct boot_sector_t
{
    char ignore1[0xe];
    unsigned short count_of_reserved; // 0x00E
    unsigned char num_of_fat;         // 0x010
    char ignore2[0x13];
    unsigned int sectors_per_fat; // 0x024
    char ignore3[4];
    unsigned int cluster_num_of_root; // 0x02c
} __attribute__((packed)) boot_sector_t;

typedef struct fat32_info_t
{
    unsigned int lba;
    unsigned int size;
    unsigned short count_of_reserved;
    unsigned char num_of_fat;
    unsigned int sectors_per_fat;
    unsigned int cluster_num_of_root;
    unsigned int offset;
} fat32_info_t;

static fat32_info_t fat32_info;

typedef struct file_info_t
{
    char name[8];            // 0x00
    char extension[3];       // 0x08
    char ignore1[9];         // 0x0b
    unsigned short start_hi; // 0x14
    char ignore2[4];         // 0x16
    unsigned short start_lo; // 0x1a
    unsigned int size;       // 0x1c
} __attribute__((packed)) file_info_t;

void fat32_memcpy(char *dest, const char *src, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

int fat_getpartition(void)
{
    char buf[512];
    int lba, size;

    print_block(0);
    // read the partitioning table

    mbr_partition_t part;
    readblock(0, &buf);
    fat32_memcpy((char *)&part, (char *)&buf, 512);

    printf("lba\n\r");

    /*
    lba = part.lba;
    size = part.size;
    */

    lba = 2048;
    size = 0;
    printf("lba: %d, size: %d\n\r", lba, size);
    boot_sector_t b;
    readblock(lba, &buf);
    print_block(lba);

    fat32_memcpy((char *)&b, (char *)&buf, 512);

    printf("cluster_num_of_root %d\n\r", b.cluster_num_of_root);
    printf("count_of_reserved %d\n\r", b.count_of_reserved);
    printf("sectors_per_fat %d\n\r", b.sectors_per_fat);
    printf("num_of_fat %d\n\r", b.num_of_fat);

    print_block(b.count_of_reserved * b.cluster_num_of_root);

    int offset = lba + b.count_of_reserved + b.num_of_fat * b.sectors_per_fat;
    print_block(offset);

    fat32_info.cluster_num_of_root = b.cluster_num_of_root;
    fat32_info.num_of_fat = b.num_of_fat;
    fat32_info.sectors_per_fat = b.sectors_per_fat;
    fat32_info.count_of_reserved = b.count_of_reserved;
    fat32_info.offset = offset;

    fat32_root = b.cluster_num_of_root;
    return 0;
}

int fat32_lookup(unsigned int dir_cluster, const char *filename, file_info_t *r_file_info)
{
    char buf[512];
    char dir_buf[512];
    file_info_t file_info;

    int offset = fat32_info.offset - dir_cluster + fat32_info.cluster_num_of_root;
    readblock(offset, &dir_buf);

    printf("fat32 lookup:\n\r");

    int end_flag = 0;
    for (int i = 0; i < 512 / sizeof(file_info); i++)
    {
        fat32_memcpy((char *)&file_info, (char *)&dir_buf[sizeof(file_info) * i], sizeof(file_info));

        end_flag = 1;
        char name[14];
        int count = 0;
        for (int i = 0; i < 8; i++)
        {
            if (file_info.name[i] != 0 && file_info.name[i] != 0x20)
            {
                end_flag = 0;
                name[count++] = file_info.name[i];
            }
        }
        if (end_flag)
            break;
        name[count++] = '.';
        for (int i = 0; i < 3; i++)
        {
            if (file_info.extension[i] != 0 && file_info.extension[i] != 0x20)
            {
                name[count++] = file_info.extension[i];
            }
        }
        name[count] = '\0';
        printf("\t%s\n\r", name);
        int value = (file_info.start_hi << 16) + file_info.start_lo;
        printf("\tsector: %08x, size: %d\n\r", value, file_info.size);

        if (strcmp(name, filename) == 0)
        {
            printf("find %s\n\r", filename);
            *r_file_info = file_info;
            return 0;
        }
    }
    printf("not find %s\n\r", filename);
    return -1;
}

void fat32_readfile(unsigned int dir_cluster, const char *filename)
{
    char buf[512];
    file_info_t file_info;

    printf("fat32 readfile:\n\r");
    fat32_lookup(fat32_info.cluster_num_of_root, filename, &file_info);

    printf("open %s\n\r", filename);
    printf("%d %d %d %d\n\r", sizeof(file_info), file_info.start_hi, file_info.start_lo, file_info.size);

    int value = (file_info.start_hi << 16) + file_info.start_lo;

    int offset = fat32_info.offset + value - fat32_info.cluster_num_of_root;
    readblock(offset, &buf);
    print_block(offset);
}

int fat32_read(unsigned int dir_cluster, const char *filename, char *r_buf, int start, int len)
{
    char buf[512];
    file_info_t file_info;

    int rvalue = fat32_lookup(fat32_info.cluster_num_of_root, filename, &file_info);

    if (rvalue == 0)
    {
        int value = (file_info.start_hi << 16) + file_info.start_lo;
        int offset = fat32_info.offset + value - fat32_info.cluster_num_of_root;

        readblock(offset, &buf);
        print_block(offset);

        int count = 0;
        for (int i = 0; i < len; i++)
        {
            r_buf[i] = buf[start + count++];
        }
    }
    return rvalue;
}

int fat32_write(unsigned int dir_cluster, const char *filename, char *w_buf, int start, int len)
{
    char buf[512];
    file_info_t file_info;

    int rvalue = fat32_lookup(fat32_info.cluster_num_of_root, filename, &file_info);

    if (rvalue == 0)
    {

        int value = (file_info.start_hi << 16) + file_info.start_lo;
        int offset = fat32_info.offset + value - fat32_info.cluster_num_of_root;

        for (int i = 0; i < len; i++)
        {
            buf[start + i] = w_buf[i];
        }
        writeblock(offset, &buf);
    }
    return rvalue;
}

int fat32_mount()
{
    sd_init();
    fat_getpartition();
}
#endif