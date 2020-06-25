#include "fat32.h"
#include <stdint.h>
#include "uart.h"
#include "mbr.h"
#include "slab.h"
#include "sd.h"

static struct vfs_vnode_operations_struct * fat32_vnode_ops;
static struct vfs_file_operations_struct * fat32_file_ops;

struct vfs_filesystem_struct * fat32_init(void)
{
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));
  fs -> name = "fat32";

  fat32_vnode_ops = (struct vfs_vnode_operations_struct *)slab_malloc(sizeof(struct vfs_vnode_operations_struct));
  fat32_file_ops = (struct vfs_file_operations_struct *)slab_malloc(sizeof(struct vfs_file_operations_struct));

  /* TODO: Setup function pointer */
  fs -> setup_mount = fat32_setup_mount;

  vfs_regist_fs(fs);

  return fs;
}

int fat32_setup_mount(struct vfs_filesystem_struct * fs, struct vfs_mount_struct * mount)
{
  UNUSED(fs);

  uint8_t block_buf[512];

  /* Read MBR partition */
  struct mbr_partition_entry_struct partition_entry;

  mbr_get_primary_partition(&partition_entry, 0);

  struct fat32_metadata_struct * metadata = (struct fat32_metadata_struct *)slab_malloc(sizeof(struct fat32_metadata_struct));

  metadata -> lba_start = mbr_chs_to_lba(partition_entry.start_c, partition_entry.start_h, partition_entry.start_s);
  metadata -> lba_end = mbr_chs_to_lba(partition_entry.end_c, partition_entry.end_h, partition_entry.end_s);

  sd_readblock((int)metadata -> lba_start, block_buf);

  metadata -> sectors_per_cluster = block_buf[FAT32_SECTORS_PER_CLUSTER_OFFSET];
  metadata -> reserved_sectors_count = *(uint16_t *)(&block_buf[FAT32_RESERVED_SECTORS_COUNT_OFFSET]);
  metadata -> number_of_fat = block_buf[FAT32_NUMBER_OF_FAT_OFFSET];
  metadata -> sectors_per_fat = *(uint32_t *)(&block_buf[FAT32_SECTORS_PER_FAT_OFFSET]);
  metadata -> root_dir_cluster_number = *(uint32_t *)(&block_buf[FAT32_CLUSTER_NUMBER_OF_ROOT_OFFSET]);

  metadata -> fat1_offset = (metadata -> reserved_sectors_count * FAT32_BYTES_PER_SECTOR) + (metadata -> lba_start * FAT32_BYTES_PER_SECTOR);
  metadata -> fat_size = metadata -> sectors_per_fat * FAT32_BYTES_PER_SECTOR;

  if(metadata -> number_of_fat == 2)
  {
    metadata -> fat2_offset = metadata -> fat1_offset + metadata -> fat_size;
    metadata -> data_offset = metadata -> fat2_offset + metadata -> fat_size;
  }
  else
  {
    metadata -> data_offset = metadata -> fat1_offset + metadata -> fat_size;
  }

  mount -> root = fat32_create_vnode(mount, (void *)(metadata -> fat1_offset) + (metadata -> root_dir_cluster_number *FAT32_BYTES_PER_CLUSTER), 1);

  return 0;
}

struct vfs_vnode_struct * fat32_create_vnode(struct vfs_mount_struct * mount, void * internal, int is_dir)
{
  struct vfs_vnode_struct * target_vnode = (struct vfs_vnode_struct *)slab_malloc(sizeof(struct vfs_vnode_struct));
  target_vnode -> mount = mount;
  target_vnode -> v_ops = fat32_vnode_ops;
  target_vnode -> f_ops = fat32_file_ops;
  target_vnode -> internal = internal;
  target_vnode -> is_dir = is_dir;
  return target_vnode;
}


