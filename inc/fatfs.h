#ifndef FATFS_H
#define FATFS_H
#define DIR_MAX 10
#define FAT_BUF_SIZE 512
#include <stdint.h>

#include "vfs.h"
struct fat_BS {
    unsigned char bootjmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;
    unsigned int table_size_32;
    unsigned short extended_flags;
    unsigned short fat_version;
    unsigned int root_cluster;
    unsigned short fat_info;
    unsigned short backup_BS_sector;
    unsigned char reserved_0[12];
    unsigned char drive_number;
    unsigned char reserved_1;
    unsigned char boot_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fat_type_label[8];
} __attribute__((packed));

struct fat_root {
    unsigned char filename[8];
    unsigned char extension[3];
    uint8_t file_attr;
    unsigned char reserved;
    uint8_t created_stamp;
    unsigned short created_time;
    unsigned short created_date;
    unsigned short last_date;
    unsigned short cluster_h;
    unsigned short last_write_time;
    unsigned short last_write_date;
    unsigned short cluster_l;
    unsigned int file_size;
} __attribute__((packed));

#define TMP_BUF_SIZE 512
#include "vfs.h"

struct fatfs_buf {
    int flag;
    size_t size;
    char buffer[FAT_BUF_SIZE];
};

struct fatentry {
    char name[15];
    unsigned int cluster_id;
    FILE_TYPE type;
    struct vnode* vnode;
    struct fatentry* list[DIR_MAX];
    struct fatentry* parent;
    struct fatfs_buf* buf;
};

extern struct file_operations* fatfs_f_ops;
extern struct vnode_operations* fatfs_v_ops;

extern int fat_base;
extern int sectors_per_cluster;
extern int first_data_sector;
extern struct file_operations* fatfs_f_ops;
extern struct vnode_operations* fatfs_v_ops;

int get_first_sector(int cluster);

int fatfs_mount(struct filesystem* fs, struct mount* mount);
int fatfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name);
int fatfs_write(struct file* file, const void* buf, size_t len);
int fatfs_read(struct file* file, void* buf, size_t len);
int fatfs_list(struct file* file);
#endif