#include <stdint.h>
#include "vfs.h"
#ifndef __FAT32_H__
#define __FAT32_H__

#define FAT32_PARTITION_ID 0xb

#define FAT32_SECTORS_PER_CLUSTER_OFFSET 0xd /* located in BIOS Parameter Block */
#define FAT32_RESERVED_SECTORS_COUNT_OFFSET 0xe /* located in BIOS Parameter Block */
#define FAT32_NUMBER_OF_FAT_OFFSET 0x10 /* located in BIOS Parameter Block */
#define FAT32_SECTORS_PER_FAT_OFFSET 0x24 /* located in FAT32 Extended BIOS Parameter Block */
#define FAT32_CLUSTER_NUMBER_OF_ROOT_OFFSET 0x2c /* located in FAT32 Extended BIOS Parameter Block */ /* typically 2 */

#define FAT32_BYTES_PER_CLUSTER 4u
#define FAT32_BYTES_PER_SECTOR 512u /* TODO: Retrieve from FAT32 */
#define FAT32_FILE_ENTRY_SIZE 32u
#define FAT32_CLUSTER_DATA_SECTION_OFFSET 2

#define FAT32_FILE_NAME_OFFSET 0x0
#define FAT32_FILE_EXTENSION_OFFSET 0x8
#define FAT32_FILE_ATTRIBUTE_OFFSET 0xb
#define FAT32_FILE_CLUSTER_HIGH_TWO_BYTES_OFFSET 0x14
#define FAT32_FILE_CLUSTER_LOW_TWO_BYTES_OFFSET 0x1a
#define FAT32_FILE_SIZE_OFFSET 0x1c

struct fat32_metadata_struct
{
  uint64_t fat_size, fat1_offset, fat2_offset, data_offset;
  uint32_t lba_start, lba_end, sectors_per_fat, root_dir_cluster_number;
  uint16_t reserved_sectors_count;
  uint8_t number_of_fat, sectors_per_cluster;
};

struct fat32_file_struct
{
  char name[9]; /* Add null byte */
  char extension[4]; /* Add null byte */
  uint8_t is_dir;
  uint32_t start_of_file;
  uint32_t filesize;
};

struct vfs_filesystem_struct * fat32_init(void);
int fat32_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);
int fat32_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name);
int fat32_read(struct vfs_file_struct * file, void * buf, size_t len);

struct vfs_vnode_struct * fat32_create_vnode(struct vfs_mount_struct * mount, void * internal, int is_dir);
void fat32_get_sd_block_and_offset(uint64_t location, uint64_t * block, uint64_t * offset);
int fat32_get_file_entry(struct fat32_file_struct * file_struct, uint8_t * base, unsigned entry_idx);

#endif

