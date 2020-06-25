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

struct fat32_metadata_struct
{
  uint64_t fat_size, fat1_offset, fat2_offset, data_offset;
  uint32_t lba_start, lba_end, sectors_per_fat, root_dir_cluster_number;
  uint16_t reserved_sectors_count;
  uint8_t number_of_fat, sectors_per_cluster;
};

struct vfs_filesystem_struct * fat32_init(void);
int fat32_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount);

struct vfs_vnode_struct * fat32_create_vnode(struct vfs_mount_struct * mount, void * internal, int is_dir);

#endif

