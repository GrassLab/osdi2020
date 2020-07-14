#ifndef FAT32
#define FAT32
#include "config.h"

struct directory_entry
{
  char name[8];			      // 0x00
  char extension[3];		  // 0x08
  char ignore1[9];		    // 0x0b
  unsigned short start_hi;// 0x14
  char ignore2[4];		    // 0x16
  unsigned short start_lo;// 0x1a
  unsigned int size;		  // 0x1c
} __attribute__ ((packed));

struct boot_sector
{
  char ignore1[0xe];
  unsigned short count_of_reserved;	// 0xe
  unsigned char fat_num;	          // 0x10
  char ignore2[0x13];
  unsigned int sectors_num_per_fat;	// 0x24
  char ignore3[4];
  unsigned int root_clstr_index;	  // 0x2c
} __attribute__ ((packed));

struct partition_entry
{
  char ignore[0x1c6];
  unsigned int logical_block_address;
  unsigned int size;
} __attribute__ ((packed));

struct fat32_info
{
  unsigned int logical_block_address;
  unsigned int size;
  unsigned short count_of_reserved;
  unsigned char fat_num;
  unsigned int sectors_num_per_fat;
  unsigned int root_clstr_index;
};

struct fat32_node
{
  struct fat32_info info;
  unsigned int cluster_index;
  unsigned int dir_entry;
  unsigned int dir_index;
};


typedef struct directory_entry directory_entry_t;
typedef struct boot_sector boot_sector_t;
typedef struct partition_entry partition_entry_t;
typedef struct fat32_info fat32_info_t;
typedef struct fat32_node fat32_node_t;

void fat32_init();
void fat32_ls(vnode_t *node);
filesystem_t *fat32_fs;

#define BLOCK_SIZE 512
#define CHAIN_LENGTH (BLOCK_SIZE/sizeof(unsigned int))
// #define CHAIN_EOF 0xffffff8
#define CHAIN_EOF 0xfffffff
#define DIR_LEN (BLOCK_SIZE/sizeof(struct directory_entry))


int write(file_t *file, const void *buf, size_t len);
int read(file_t *file, void *buf, size_t len);
int create(vnode_t *dir_node, vnode_t **target, const char *component_name);
int lookup(vnode_t *dir_node, vnode_t **target, const char *component_name);
fat32_node_t *internal_node_create(fat32_node_t *parent,
                                    unsigned int cluster_index,
                                    unsigned int dir_entry,
                                    unsigned int dir_index);

unsigned int calculated_offset(fat32_node_t *node);

#endif 