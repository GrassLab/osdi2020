#ifndef KERNEL_FAT32_H_
#define KERNEL_FAT32_H_

#include "kernel/lib/types.h"
#include "kernel/vfs.h"

#define MAX_FILE_SIZE 512
#define MAX_FILE_PER_DIR 16
#define MAX_FILENAME_LEN 256

#define BLOCK_SIZE 512
#define EOC 0x0fffffff

extern void sd_init(void);
extern void readblock(int block_idx, void* buf);
extern void writeblock(int block_idx, void* buf);

struct filesystem fat32;
struct fat32_meta fat32_meta;
struct file_operations fat32_file_operations;
struct vnode_operations fat32_vnode_operations;

struct fat32_meta {
  /* BIOS Parameter Block */
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sector_nums;
  uint8_t file_allocation_table_nums;
  uint32_t sectors_per_fat;
};

struct fat32_dirent {
  struct vnode *vn;
  /* Directory Table Entry */
  char name[13];  // 0x00 and 0x08
  uint8_t attrs;  // 0x0b
  uint32_t cluster_num; // 0x14 || 0x1a
  uint32_t size;  // 0x1c

  /* Used for file to update its size in directory entry */
  uint32_t parent_cluster_num;
};

void read_partition_block(int block_idx, void* buf);
void write_partition_block(int block_idx, void* buf);

int fat32_setup_mount(struct filesystem* fs, struct mount* mount);

int fat32_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);

int fat32_write(struct file* file, const void* buf, size_t len);
int fat32_read(struct file* file, void* buf, size_t len);

void gen_filename(const char *sfn, const char *sfe, char *filename);
uint32_t get_fat_entry(uint32_t cluster_num);
void set_fat_entry(uint32_t cluster_num, uint32_t val);
void read_data_region(uint32_t cluster_num, char *buf);
void write_data_region(uint32_t cluster_num, char *buf);
uint32_t get_unused_cluster(void);
void update_file_cluster_num(uint32_t dir_cluster_num, const char *filename, uint32_t cluster_num);
void update_file_size(uint32_t dir_cluster_num, const char *filename, size_t size);

#endif // KERNEL_FAT32_H_
