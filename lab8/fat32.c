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
  fat32_vnode_ops -> create = fat32_create;

  fat32_file_ops -> read = fat32_read;
  fat32_file_ops -> write = fat32_write;

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
    char filename[16];
    file_struct -> entry_location = (uint64_t)(data_offset + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE);

    if(fat32_get_file_entry(file_struct, sd_buf, entry_idx) == 0)
    {
      continue;
    }

    filename[0] = '\0';
    string_concat(filename, file_struct -> name);
    if(string_length(file_struct -> extension) != 0)
    {
      string_concat(filename, ".");
      string_concat(filename, file_struct -> extension);
    }

    if(string_cmp(filename, component_name, 16) != 0)
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

int fat32_read(struct vfs_file_struct * file, void * buf, size_t len)
{
  struct fat32_file_struct * file_struct = (struct fat32_file_struct *)file -> vnode -> internal;
  uint8_t sd_buf[512];
  uint64_t block_idx, block_offset;
  unsigned read_offset = (unsigned)(file -> read_pos);
  int read_length;

  if(len > (file_struct -> filesize - read_offset - 1))
  {
    read_length = (signed)(file_struct -> filesize - read_offset);
  }
  else
  {
    read_length = (int)len;
  }

    int cumulated_length = 0;
    uint64_t next_file_cluster = file_struct -> start_of_file;
    while(cumulated_length < read_length)
    {
      int read_chunk = read_length - cumulated_length - 512 > 0 ? 512 : read_length - cumulated_length;
      /*  Read from data_offset */
      fat32_get_sd_block_and_offset((next_file_cluster - FAT32_CLUSTER_DATA_SECTION_OFFSET) * FAT32_BYTES_PER_SECTOR * fat32_metadata.sectors_per_cluster + fat32_metadata.data_offset, &block_idx, &block_offset);
      sd_readblock((int)block_idx, sd_buf);
      memcopy((const char *)(sd_buf + read_offset), (char *)(buf + cumulated_length), (unsigned)read_chunk);

      cumulated_length += read_chunk;

      if((next_file_cluster & 0xffffff8) != 0xffffff8)
      {
        /* get the cluster number of next sector */
        fat32_get_sd_block_and_offset(next_file_cluster * FAT32_BYTES_PER_CLUSTER  + fat32_metadata.fat1_offset, &block_idx, &block_offset);
        sd_readblock((int)block_idx, sd_buf);
        next_file_cluster = *(uint32_t *)(sd_buf + block_offset);
      }
    }

  return read_length;

}

int fat32_write(struct vfs_file_struct * file, const void * buf, size_t len)
{
  struct fat32_file_struct * file_struct = (struct fat32_file_struct *)file -> vnode -> internal;
  uint8_t sd_buf[512];
  uint64_t block_idx, block_offset;
  unsigned write_offset = (unsigned)(file -> write_pos);

  size_t cumulated_length = 0;
  uint32_t current_file_cluster = file_struct -> start_of_file;

  while(cumulated_length < len)
  {
    size_t write_chunk = len - cumulated_length > 512 ? 512 : len - cumulated_length;

    if((current_file_cluster & 0xfffffff) == 0)
    {
      /* acquire a cluster for write */
      current_file_cluster = fat32_append_cluster(file_struct -> entry_location);
      file_struct -> start_of_file = current_file_cluster;
    }

    /* else write file */

    /* Write to data_offset */
    fat32_get_sd_block_and_offset((current_file_cluster - FAT32_CLUSTER_DATA_SECTION_OFFSET) * FAT32_BYTES_PER_SECTOR * fat32_metadata.sectors_per_cluster + fat32_metadata.data_offset, &block_idx, &block_offset);
    memcopy((const char *)(buf + cumulated_length), (char *)(sd_buf + write_offset), (unsigned)write_chunk);
    sd_writeblock((unsigned)block_idx, sd_buf);

    cumulated_length += write_chunk;

    if(cumulated_length < len)
    {
      /* get the next cluster */
      uint32_t next_file_cluster;
      fat32_get_sd_block_and_offset(fat32_metadata.fat1_offset + current_file_cluster * FAT32_BYTES_PER_CLUSTER, &block_idx, &block_offset);
      sd_readblock((int)block_idx, sd_buf);
      next_file_cluster = *(uint32_t *)(sd_buf + block_offset);

      /* there will be next loop, current file is not long enough, find another cluster for write */
      if((next_file_cluster & 0xffffff8) == 0xffffff8)
      {
        current_file_cluster = fat32_append_cluster(file_struct -> entry_location);
      }
      else
      {
        current_file_cluster = next_file_cluster;
      }
    }

    /* TODO: Free the cluster if the file is not that long */
  }

  /* Update filesize */
  file_struct -> filesize = (uint32_t)cumulated_length;

  fat32_get_sd_block_and_offset(file_struct -> entry_location, &block_idx, &block_offset);
  sd_readblock((int)block_idx, sd_buf);

  *(uint32_t *)(sd_buf + block_offset + FAT32_FILE_SIZE_OFFSET) = (uint32_t)cumulated_length;
  sd_writeblock((unsigned)block_idx, sd_buf);

  return (int)cumulated_length;
}

int fat32_create(struct vfs_vnode_struct * dir_node, struct vfs_vnode_struct ** target, const char * component_name)
{
  /* TODO: Support for multi level search */
  /* TODO: Create cluster if the current directory cluster is full */

  uint8_t sd_buf[512];
  uint64_t block_idx, block_offset, data_offset;

  data_offset = ((((uint64_t)(dir_node -> internal) - fat32_metadata.fat1_offset) / FAT32_BYTES_PER_CLUSTER) - FAT32_CLUSTER_DATA_SECTION_OFFSET) * FAT32_BYTES_PER_SECTOR * fat32_metadata.sectors_per_cluster + fat32_metadata.data_offset;

  fat32_get_sd_block_and_offset((uint64_t)data_offset, &block_idx, &block_offset);
  sd_readblock((int)block_idx, sd_buf);

  unsigned total_entries = fat32_metadata.sectors_per_cluster * FAT32_BYTES_PER_SECTOR / FAT32_FILE_ENTRY_SIZE;
  struct fat32_file_struct * file_struct = (struct fat32_file_struct *)slab_malloc(sizeof(struct fat32_file_struct));
  for(unsigned entry_idx = 0; entry_idx < total_entries; ++entry_idx)
  {
    file_struct -> entry_location = (uint64_t)(data_offset + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE);

    if(fat32_get_file_entry(file_struct, sd_buf, entry_idx) != 0)
    {
      continue;
    }

    /* empty slot found, fill in the file_struct first */

    /* split the filename into 8.3 */
    char filename[16];
    int dot_pos = -1;
    string_copy(component_name, filename);

    /* Set to uppercase */
    for(int i = 0; filename[i] != '\0'; ++i)
    {
      if('a' <= filename[i] && filename[i] <= 'z')
      {
#pragma GCC diagnostic ignored "-Wconversion"
        filename[i] -= 'a' + 'A';
      }
    }

    for(int i = 0; i < 12; ++i)
    {
      if(filename[i] == '.')
      {
        dot_pos = i;
        filename[i] = '\0';
        break;
      }
    }

    if(dot_pos == -1)
    {
      /* no extension */
      string_copy(filename, file_struct -> name);
    }
    else
    {
      string_copy(filename, file_struct -> name);
      string_copy(filename + dot_pos + 1, file_struct -> extension);
    }

    file_struct -> is_dir = 0;
    file_struct -> start_of_file = 0;
    file_struct -> filesize = 0;

    /* Write back entry to disk */
    /* clear out the entry first */
    /* 32 bytes = 8 byte * 4 */
    memzero_8byte((uint64_t *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE), 4);

    /* fill in the entry */
    /* append name and extension with space */
    string_copy(file_struct -> name, (char *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_NAME_OFFSET));
    for(int i = string_length(file_struct -> name); i < 8; ++i)
    {
      *(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_NAME_OFFSET + i) = ' ';
    }

    string_copy(file_struct -> extension, (char *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_EXTENSION_OFFSET));
    for(int i = string_length(file_struct -> extension); i < 3; ++i)
    {
      *(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_EXTENSION_OFFSET + i) = ' ';
    }

    /* set file attribute to 0 */
    *(uint8_t *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_ATTRIBUTE_OFFSET) = 0;

    /* set start of cluster to 0 */
    *(uint16_t *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_CLUSTER_HIGH_TWO_BYTES_OFFSET) = 0;
    *(uint16_t *)(sd_buf + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_CLUSTER_LOW_TWO_BYTES_OFFSET) = 0;

    /* write back to sd */
    sd_writeblock((unsigned)block_idx, sd_buf);

    *target = fat32_create_vnode(dir_node -> mount, (void *)file_struct, file_struct -> is_dir);

    return 0;
  }

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
    /* entry availible */
    return 0;
  }
  if(*(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE) == 0xe5)
  {
    /*  Entry has been previously erased and/or is available, ignored for now */
    return 0;
  }

  memcopy((const char *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_NAME_OFFSET), file_struct -> name, 8);
  /* file name has trailing space, change the leftmost trailing space to null byte */
  for(int i = 7; i >= 0; --i)
  {
    if((file_struct -> name)[i] == ' ')
    {
      if(i == 0)
      {
        (file_struct -> extension)[0] = '\0';
      }
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
      if(i == 0)
      {
        (file_struct -> extension)[0] = '\0';
      }
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

  file_struct -> filesize = *(uint32_t *)(base + (unsigned)entry_idx * FAT32_FILE_ENTRY_SIZE + FAT32_FILE_SIZE_OFFSET);

  return 1;
}

uint32_t fat32_find_availible_cluster(void)
{
  /* Will mark the availible cluster as end of cluster */

  uint8_t sd_buf[512];
  uint64_t block_idx, block_offset;

  for(unsigned fat_sector_idx = 0; fat_sector_idx < fat32_metadata.sectors_per_fat; ++fat_sector_idx)
  {
    uint32_t sub_cluster_idx = 0;

    fat32_get_sd_block_and_offset(fat32_metadata.fat1_offset + fat_sector_idx * FAT32_BYTES_PER_SECTOR, &block_idx, &block_offset);
    sd_readblock((int)block_idx, sd_buf);

    /* ignore the first two reserved cluster in the first fat sector */
    if(fat_sector_idx == 0)
    {
      sub_cluster_idx = 2;
    }

    for(; sub_cluster_idx < FAT32_BYTES_PER_SECTOR / FAT32_BYTES_PER_CLUSTER; ++sub_cluster_idx)
    {
      if(((*(uint32_t *)(sd_buf + sub_cluster_idx * FAT32_BYTES_PER_CLUSTER)) & 0xfffffff) == 0)
      {
        /* mark the availible cluster as end of cluster */
        *(uint32_t *)(sd_buf + sub_cluster_idx * FAT32_BYTES_PER_CLUSTER) = 0xfffffff;
        sd_writeblock((unsigned)block_idx, sd_buf);
        return fat_sector_idx * (FAT32_BYTES_PER_SECTOR / FAT32_BYTES_PER_CLUSTER) + sub_cluster_idx;
      }
    }
  }
  return 0;
}

uint32_t fat32_append_cluster(uint64_t entry_base)
{
  uint8_t sd_buf[512];
  uint64_t block_idx, block_offset;
  uint32_t start_of_file;
  uint32_t new_cluster;

  fat32_get_sd_block_and_offset(entry_base, &block_idx, &block_offset);
  sd_readblock((int)block_idx, sd_buf);

  start_of_file = (uint32_t)(*(uint16_t *)(sd_buf + block_offset + FAT32_FILE_CLUSTER_HIGH_TWO_BYTES_OFFSET)) << 16;
  start_of_file |= *(uint16_t *)(sd_buf + block_offset + FAT32_FILE_CLUSTER_LOW_TWO_BYTES_OFFSET);

  new_cluster = fat32_find_availible_cluster();

  if(start_of_file == 0)
  {
    *(uint16_t *)(sd_buf + block_offset + FAT32_FILE_CLUSTER_HIGH_TWO_BYTES_OFFSET) = (uint16_t)(new_cluster >> 16);
    *(uint16_t *)(sd_buf + block_offset + FAT32_FILE_CLUSTER_LOW_TWO_BYTES_OFFSET) = (new_cluster & 0xffff);
    sd_writeblock((unsigned)block_idx, sd_buf);
    /* TODO: Update the second fat table */
  }
  else
  {
    uint64_t prev_block_idx = 0;
    uint32_t cluster_idx = start_of_file;
    /* traverse the list */
    while((cluster_idx & 0xffffff8) != 0xffffff8)
    {
      fat32_get_sd_block_and_offset(fat32_metadata.fat1_offset + cluster_idx * FAT32_BYTES_PER_CLUSTER, &block_idx, &block_offset);
      if(block_idx != prev_block_idx)
      {
        prev_block_idx = block_idx;
        sd_readblock((int)block_idx, sd_buf);
      }
      cluster_idx = *(uint32_t *)(sd_buf + block_offset);
    }

    /* reach the end of list */
    *(uint32_t *)(sd_buf + block_offset) = new_cluster;

    /* write back */
    sd_writeblock((unsigned)block_idx, sd_buf);

    /* TODO: Update the second fat table */
  }

  return new_cluster;
}

