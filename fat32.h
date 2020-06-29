#ifndef _FAT32_H_
#define _FAT32_H_

#include "sdhost.h"
#include "printf.h"
#include "string.h"

// get the end of bss segment from linker
extern unsigned char _end;

static unsigned int partitionlba = 0;

// the BIOS Parameter Block (in Volume Boot Record)
typedef struct
{
    char jmp[3];
    char oem[8];
    unsigned char bps0;
    unsigned char bps1;
    unsigned char spc;
    unsigned short rsc;
    unsigned char nf;
    unsigned char nr0;
    unsigned char nr1;
    unsigned short ts16;
    unsigned char media;
    unsigned short spf16;
    unsigned short spt;
    unsigned short nh;
    unsigned int hs;
    unsigned int ts32;
    unsigned int spf32;
    unsigned int flg;
    unsigned int rc;
    char vol[6];
    char fst[8];
    char dmy[20];
    char fst2[8];
} __attribute__((packed)) bpb_t;

// directory entry structure
typedef struct
{
    char name[8];
    char ext[3];
    char attr[9];
    unsigned short ch;
    unsigned int attr2;
    unsigned short cl;
    unsigned int size;
} __attribute__((packed)) fatdir_t;

void print_block(int block)
{
    unsigned char buf[512];
    readblock(block, (unsigned char *)&buf);

    for (int i = 0; i < 512; i += 16)
    {
        uart_send_hex(i);
        uart_puts(": ");
        for (int j = 0; j < 16; j += 2)
        {
            printf("%2x", (buf[i + j]));
            printf("%2x ", (buf[i + j + 1]));
        }
        printf("\n");
    }
}

/**
 * Get the starting LBA address of the first partition
 * so that we know where our FAT file system starts, and
 * read that volume's BIOS Parameter Block
 */
int fat_getpartition(void)
{
    bpb_t bpb;
    unsigned char buf[512];
    unsigned char *mbr = buf;

    print_block(0);
    // read the partitioning table
    readblock(0, (unsigned char *)&buf);

    // check magic
    if (mbr[510] != 0x55 || mbr[511] != 0xAA)
    {
        uart_puts("ERROR: Bad magic in MBR\n");
        return 0;
    }
    uart_send_hex(mbr[0x1c2]);
    uart_puts("MBR disk identifier: ");
    //uart_send_hex(*((unsigned int *)((unsigned long)&_end + 0x1B8)));
    uart_send_hex(mbr[0x1B8]);
    uart_puts("\nFAT partition starts at: ");
    // should be this, but compiler generates bad code...
    //partitionlba=*((unsigned int*)((unsigned long)&_end+0x1C6));
    partitionlba = mbr[0x1C6] + (mbr[0x1C7] << 8) + (mbr[0x1C8] << 16) + (mbr[0x1C9] << 24);
    uart_send_hex(partitionlba);
    uart_puts("\n");
    // read the boot record
    //readblock(partitionlba, &_end, 1);
    readblock(1, (unsigned char *)&_end);

    print_block(1);
    readblock(1, (unsigned char *)&buf);
    memcpy((unsigned char *)&bpb, &buf, sizeof(bpb));

    uart_send('\n');
    uart_send(bpb.fst[0]);
    uart_send(bpb.fst[1]);
    uart_send(bpb.fst[2]);
    uart_send('\n');

    // check file system type. We don't use cluster numbers for that, but magic bytes
    if (!(bpb.fst[0] == 'F' && bpb.fst[1] == 'A' && bpb.fst[2] == 'T') &&
        !(bpb.fst2[0] == 'F' && bpb.fst2[1] == 'A' && bpb.fst2[2] == 'T'))
    {
        uart_puts("ERROR: Unknown file system type\n");
        return 0;
    }
    uart_puts("FAT type: ");
    // if 16 bit sector per fat is zero, then it's a FAT32
    uart_puts(bpb.spf16 > 0 ? "FAT16" : "FAT32");
    uart_puts("\n");
    return 1;
}

int fat32_init()
{
    sd_init();
    fat_getpartition();
}
#endif