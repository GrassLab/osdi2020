#ifndef _FAT_H
#define _FAT_H

#include "vfs.h"

#define BLOCK_SIZE 512
#define FAT_FILE_SIZE REGBUFF_SIZE
#define PARTITION_TYPE_FAT32 0xB
#define FIRST_PART_ENTRY_OFFSET 0x1BE


typedef struct partition_entry {
    unsigned char status_flag;                  //0x0
    unsigned char partition_begin_head;         //0x1
    unsigned short partition_begin_sector;      //0x2-0x3
    unsigned char partition_type;               //0x4
    unsigned char partition_end_head;           //0x5
    unsigned short partition_end_sector;        //0x6-0x7
    unsigned int starting_sector;               //0x8-0xB
    unsigned int nr_sector;                     //0xC-0xF
} __attribute__ ( ( packed ) ) partition_entry_t;

typedef struct boot_sector{
      char jump[3];                               // 0x0
      char oem[8];                                // 0x3
    unsigned short bytes_per_logical_sector;    // 0xB 0xC
    unsigned char sector_per_cluster;           // 0xD
    unsigned short nr_reserved_sectors;         // 0xE
    unsigned char nr_fat32_table;               // 0x10
      unsigned short n_root_dir_entries;          // 0x11-0x12
      unsigned short n_logical_sectors_16;        // 0x13-0x14
      unsigned char media_descriptor;             // 0x15
      unsigned short logical_sector_per_fat;      // 0x16-0x17
      unsigned short physical_sector_per_track;   // 0x18-0x19
    unsigned short nr_heads;                    // 0x1A-0x1B
    unsigned int nr_hidden_sectors;             // 0x1C-0x1F
    unsigned int nr_sectors;                    // 0x20-0x23
    unsigned int nr_sector_fat32;               // 0x24-0x27
      unsigned short mirror_flag;                 // 0x28-0x29
    unsigned short version;                     // 0x2A-0x2B
    unsigned int root_cluster;                 // 0x2C-0x2F
      unsigned short info;                        // 0x30-0x31
      unsigned short num_of_backupboot_sector;    //0x32-0x33
    unsigned int reserved[3];                   //0x34--0x3F
      unsigned char logical_dirve_num;            // 0x40
      unsigned char unused;                       //0x41
      unsigned char extend_signature;             //0x42
    

    unsigned int serial_number;                 // 0x43-0x46
    unsigned char volume_name[11];              // 0x47-0x51
    unsigned char fat_name[8];                  // 0x52-0x59
} __attribute__ ( ( packed ) ) boot_sector_t;

struct fat_dir{
    char            name[8];                    // 0x00 File name, padded with spaces
    char            ext[3];                     // 0x08 file extension
    char            attr[9];                    // 0x0b File attribute
                                                // 0x0c Reserved
                                                // 0x0d file create time.ms
                                                // 0x0e file create time.s
                                                // 0x10 file create time date
                                                // 0x12 last access date
    unsigned short  cluster_high;               // 0x14
    short            ext_attr[2];               // 0x16 time last write
                                                // 0x18 date last write
    unsigned short  cluster_low;                // 0x1a
    unsigned int    size;                       // 0x1c
};// __attribute__ ( ( packed ) ) fat_dir_t;


struct fat_internal{
    unsigned int first_cluster;
    unsigned int size;
};

int fat_getpartition();
void setup_fs_fat(struct filesystem *fs);
struct vnode *create_fat_vnode(int type, unsigned int first_cluster, unsigned int size);
int setup_mount_fat(struct filesystem *fs, struct mount *mount);
int lookup_fat(struct vnode *vnode, struct vnode **target,const char *component_name);
int create_fat(struct vnode *vnode, struct vnode **target,const char *component_name);
int read_fat(struct file *file, void *buf, unsigned len);
int write_fat(struct file *file, const void *buf, unsigned len);

#endif//_FAT_H