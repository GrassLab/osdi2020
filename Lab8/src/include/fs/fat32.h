#ifndef _FAT32_H
#define _FAT32_H

typedef struct partition_entry
{
    unsigned char status_flag; //0x0
    unsigned char partition_begin_head; //0x1
    unsigned short partition_begin_sector; //0x2-0x3
    unsigned char partition_type; //0x4
    unsigned char partition_end_head; //0x5
    unsigned short partition_end_sector; //0x6-0x7
    unsigned int starting_sector; //0x8-0xB
    unsigned int number_of_sector; //0xC-0xF

} partition_entry_t;

typedef struct boot_sector
{
    char jump[3];                      // 0x0
    char oem[8];                       // 0x3
    unsigned short bytes_per_logical_sector;  // 0xB 0xC
    unsigned char logical_sector_per_cluster;        // 0xD
    unsigned short n_reserved_sectors;       // 0xE
    unsigned char n_file_alloc_tabs;         // 0x10
    unsigned short n_root_dir_entries;      // 0x11-0x12
    unsigned short n_logical_sectors_16;     // 0x13-0x14
    unsigned char media_descriptor;                     // 0x15
    unsigned short logical_sector_per_fat;      // 0x16-0x17
    unsigned short physical_sector_per_track;     // 0x18-0x19
    unsigned short n_heads;                  // 0x1A-0x1B
    unsigned int n_hidden_sectors;         // 0x1C-0x1F
    unsigned int n_sectors_32;             // 0x20-0x23
    unsigned int n_sector_per_fat_32;      // 0x24-0x27
    unsigned short mirror_flag;              // 0x28-0x29
    unsigned short version;                  // 0x2A-0x2B
    unsigned int first_cluster;            // 0x2C-0x2F
    unsigned short info;                     // 0x30-0x31
    
    unsigned short num_of_backupboot_sector; //0x32-0x33
    unsigned int reserved[3];                   //0x34--0x3F
    unsigned char logical_dirve_num;// 0x40
    unsigned char unused; //0x41
    unsigned char extend_signature; //0x42
    

    unsigned int serial_number;                 // 0x43-0x46
    unsigned char volume_name[11];                 // 0x47-0x51
    unsigned char fat_name[8];                   // 0x52-0x59
} __attribute__ ( ( packed ) ) boot_sector_t;


typedef struct fat32_dir
{
    char name[8];           // 0x0-0x7
    char ext[3];            // 0x8-0xA
    char attr[9];           // 0xB-19
    unsigned short cluster_high;  // 0x14-0x15
    unsigned int ext_attr;      // 0x16-0x19
    unsigned short cluster_low;   // 0x1A-0x1B
    unsigned int size;          // 0x1C-0x1F

} __attribute__ ( ( packed ) ) fat32_dir_t;

struct fat32fs_node{
    char ext[4];

    unsigned int cluster;
    unsigned int size;
}; 

boot_sector_t* boot_sec;
partition_entry_t *entry1;

struct filesystem *fat32fs; 

struct vnode_operations* fat32fs_v_ops;
struct file_operations* fat32fs_f_ops;

int fat_getpartition();
int setup_mount_fat32fs(struct filesystem* fs, struct mount* mt);
#endif
