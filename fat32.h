#ifndef _FAT32_H_
#define _FAT32_H_

#include "sdhost.h"
#include "printf.h"
#include "string.h"

static unsigned int partitionlba = 0;

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
        printf("\n");
    }
    printf("===\n");
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
struct boot_sector
{
    char ignore1[0xe];
    unsigned short count_of_reserved; // 0x00E
    unsigned char num_of_fat;         // 0x010
    char ignore2[0x13];
    unsigned int sectors_per_fat; // 0x024
    char ignore3[4];
    unsigned int cluster_num_of_root; // 0x02c
} __attribute__((packed));

int fat_getpartition(void)
{
    char buf[512];
    unsigned char *mbr = buf;
    unsigned int lba, size;

    print_block(0);
    // read the partitioning table

    mbr_partition_t part;
    readblock(0, (char *)&buf);
    memcpy((char *)&part, (char *)&buf, 512);
    printf("lba: %d, size: %d\n", part.lba, part.size);

    lba = part.lba;
    size = part.size;

    bpb_t bpb;
    struct boot_sector b;
    readblock(lba, &buf);
    print_block(lba);
    memcpy((char *)&bpb, (char *)&buf, 512);
    memcpy((char *)&b, (char *)&buf, 512);
    //bpb = (void *)buf;
    printf("%d\n", buf[0x00B] * 256 + buf[0x00B + 1]);
    printf("%d\n", buf[0x024]);
    printf("cluster_num_of_root: %d\n", buf[0x02C] << 24 + buf[0x02C + 1] << 16 + buf[0x02C + 2] << 8 + buf[0x02C + 3]);
    printf("cluster_num_of_root: %d %d %d %d\n", buf[0x02C], buf[0x02C + 1], buf[0x02C + 2], buf[0x02C + 3]);
    printf("=%c%c%c=\n", bpb.fst[0], bpb.fst[1], bpb.fst[2]);

    printf("cluster_num_of_root %d\n", b.cluster_num_of_root);
    printf("count_of_reserved %d\n", b.count_of_reserved);
    printf("sectors_per_fat %d\n", b.sectors_per_fat);
    printf("num_of_fat %d\n", b.num_of_fat);

    print_block(b.count_of_reserved * b.cluster_num_of_root);

    int offset = lba + b.count_of_reserved + b.num_of_fat * b.sectors_per_fat;
    print_block(offset);
    /*
    printf("-%d-%d-\n", b.cluster_num_of_root, bpb.root_start_cluster);
    print_block(b.cluster_num_of_root);
    print_block(32768);
    printf("per cluster size: %d %d %d\n", buf[0x00B], buf[0x00B + 1], bpb.root_start_cluster);
    printf("reserved num: %d %d %d\n", buf[0x00E], buf[0x00E + 1], bpb.pos_reversed_boot);
    */

    return 0;
}

int fat32_mount()
{
    sd_init();
    fat_getpartition();
}
#endif