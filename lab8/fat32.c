#include "fat32.h"
#include <stdint.h>
#include "uart.h"
#include "mbr.h"
#include "slab.h"
#include "sd.h"
#include "string_util.h"

static struct vfs_vnode_operations_struct * fat32_vnode_ops;
static struct vfs_file_operations_struct * fat32_file_ops;
static struct fat32_metadata_struct fat32_metadata;

struct vfs_filesystem_struct * fat32_init(void)
{
  struct vfs_filesystem_struct * fs = (struct vfs_filesystem_struct *)slab_malloc(sizeof(struct vfs_filesystem_struct));
  fs -> name = "fat32";

  fat32_vnode_ops = (struct vfs_vnode_operations_struct *)slab_malloc(sizeof(struct vfs_vnode_operations_struct));
  fat32_file_ops = (struct vfs_file_operations_struct *)slab_malloc(sizeof(struct vfs_file_operations_struct));

  /* TODO: Setup function pointer */
  fs -> setup_mount = fat32_setup_mount;

  fat32_vnode_ops -> lookup = fat32_lookup;

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

  fat32_metadata.lba_start = mbr_chs_to_lba(partition_entry.start_c, partition_entry.start_h, partition_entry.start_s);
  fat32_metadata.lba_end = mbr_chs_to_lba(partition_entry.end_c, partition_entry.end_h, partition_entry.end_s);

  sd_readblock((int)fat32_metadata.lba_start, block_buf);

  fat32_metadata.sectors_per_cluster = block_buf[FAT32_SECTORS_PER_CLUSTER_OFFSET];
  fat32_metadata.reserved_sectors_count = *(uint16_t *)(&block_buf[FAT32_RESERVED_SECTORS_COUNT_OFFSET]);
  fat32_metadata.number_of_fat = block_buf[FAT32_NUMBER_OF_FAT_OFFSET];
  fat32_metadata.sectors_per_fat = *(uint32_t *)(&block_buf[FAT32_SECTORS_PER_FAT_OFFSET]);
  fat32_metadata.root_dir_cluster_number = *(uint32_t *)(&block_buf[FAT32_CLUSTER_NUMBER_OF_ROOT_OFFSET]);

  fat32_metadata.fat1_offset = (fat32_metadata.reserved_sectors_count * FAT32_BYTES_PER_SECTOR) + (fat32_metadata.lba_start * FAT32_BYTES_PER_SECTOR);
  fat32_metadata.fat_size = fat32_metadata.sectors_per_fat * FAT32_BYTES_PER_SECTOR;

  if(fat32_metadata.number_of_fat == 2)
  {
    fat32_metadata.fat2_offset = fat32_metadata.fat1_offset + fat32_metadata.fat_size;
    fat32_metadata.data_offset = fat32_metadata.fat2_offset + fat32_metadata.fat_size;
  }
  else
  {
    fat32_metadata.data_offset = fat32_metadata.fat1_offset + fat32_metadata.fat_size;
  }

  mount -> root = fat32_create_vnode(mount, (void *)(fat32_metadata.fat1_offset) + (fat32_metadata.root_dir_cluster_number * FAT32_BYTES_PER_CLUSTER), 1);

  return 0;
}

int fat32_lookup(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  /* return 0 if node should be free after use, 1 if not */

  uint8_t sd_buf[FAT32_BYTES_PER_SECTOR];
  uint64_t block_idx, block_offset, data_offset;
  uint32_t next_fat;
  /* TODO: Assume one cluster use one section */
  fat32_get_sd_block_and_offset((uint64_t)dir_node -> internal, &block_idx, &block_offset);
  sd_readblock((int)block_idx, sd_buf);

  next_fat = *(uint32_t *)&sd_buf[block_offset];

  data_offset = ((((uint64_t)(dir_node -> internal) - fat32_metadata.fat1_offset) / 4) - FAT32_CLUSTER_DATA_SECTION_OFFSET) * FAT32_BYTES_PER_SECTOR * fat32_metadata.sectors_per_cluster + fat32_metadata.data_offset;

  fat32_get_sd_block_and_offset((uint64_t)data_offset, &block_idx, &block_offset);
  sd_readblock((int)block_idx, sd_buf);


  /* TODO: if directory span multiple cluster */
  if((next_fat & 0xffffff8) == 0xffffff8)
  {
    /* end of cluster */
  }
  else
  {
    /* TODO */
  }

  unsigned total_entries = fat32_metadata.sectors_per_cluster * FAT32_BYTES_PER_SECTOR / FAT32_FILE_ENTRY_SIZE;
  struct fat32_file_struct * file_struct = (struct fat32_file_struct *)slab_malloc(sizeof(struct fat32_file_struct));
  for(unsigned entry_idx = 0; entry_idx < total_entries; ++entry_idx)
  {
    fat32_get_file_entry(file_struct, sd_buf, entry_idx);

    if(string_cmp(file_struct -> name, component_name, 10) != 0)
    {

      *target = fat32_create_vnode(dir_node -> mount, (void *)file_struct, file_struct -> is_dir);
      return 0;
    }
  }

  /* nothing found */
  slab_malloc_free((uint64_t *)file_struct);
  *target = 0;
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

void fat32_get_sd_block_and_offset(uint64_t location, uint64_t * block, uint64_t * offset)
{
  *block = location / FAT32_BYTES_PER_SECTOR;
  *offset = location % FAT32_BYTES_PER_SECTOR;
  return;
}

int fat32_get_file_entry(struct fat32_file_struct * file_struct, uint8_t * base, unsigned entry_idx)
{
  /* return 0 if entry is empty, 1 if not empty */
  if(*(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE) == 0)
  {
    return 0;
  }

  memcopy((const char *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_NAME_OFFSET), file_struct -> name, 8);
  /* file name has trailing space, change the leftmost trailing space to null byte */
  for(int i = 7; i >= 0; --i)
  {
    if((file_struct -> name)[i] == ' ')
    {
      continue;
    }
    (file_struct -> name)[i + 1] = '\0';
    break;
  }

  memcopy((const char *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_EXTENSION_OFFSET), file_struct -> extension, 3);
  /* extension has trailing space, change the leftmost trailing space to null byte */
  for(int i = 2; i >= 0; --i)
  {
    if((file_struct -> extension)[i] == ' ')
    {
      continue;
    }
    (file_struct -> extension)[i + 1] = '\0';
    break;
  }

  if(CHECK_BIT(base[(unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_ATTRIBUTE_OFFSET], 4))
  {
    file_struct -> is_dir = 1;
  }
  else
  {
    file_struct -> is_dir = 0;
  }

  file_struct -> start_of_file = (uint32_t)(*(uint16_t *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_CLUSTER_HIGH_TWO_BYTES_OFFSET)) << 16;
  file_struct -> start_of_file |= *(uint16_t *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_CLUSTER_LOW_TWO_BYTES_OFFSET);

  file_struct -> filesize = *(uint32_t *)(base + (unsigned)entry_idx + FAT32_FILE_ENTRY_SIZE + FAT32_FILE_SIZE_OFFSET);

  return 1;
}

