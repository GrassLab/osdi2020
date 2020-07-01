#include "kernel/base.h"
#include "kernel/mm.h"
#include "kernel/fat32.h"
#include "kernel/vfs.h"
#include "kernel/lib/string.h"
#include "kernel/lib/ioutil.h"

struct filesystem fat32 = {
  .name = "fat32",
  .setup_mount = fat32_setup_mount
};

struct file_operations fat32_file_operations = {
  .read = fat32_read,
  .write = fat32_write
};

struct vnode_operations fat32_vnode_operations = {
  .lookup = fat32_lookup
};

void read_partition_block(int block_idx, void* buf) {
  readblock(2048 + block_idx, buf);
}

void write_partition_block(int block_idx, void* buf) {
  writeblock(2048 + block_idx, buf);
}

int fat32_setup_mount(struct filesystem* fs, struct mount* mount) {
  sd_init();
  uint8_t buf[BLOCK_SIZE];
  read_partition_block(0, buf);

  fat32_meta.bytes_per_sector = *((uint16_t *)(buf + 0xb));
  fat32_meta.sectors_per_cluster = *(buf + 0xd);
  fat32_meta.reserved_sector_nums = *((uint16_t *)(buf + 0xe));
  fat32_meta.file_allocation_table_nums = *(buf + 0x10);
  fat32_meta.sectors_per_fat = *((uint32_t *)(buf + 0x24));

  uint32_t rootdir_cluster_num = *((uint32_t *)(buf + 0x2c));
  printk("bytes_per_sector = %u\n", fat32_meta.bytes_per_sector);
  printk("sectors_per_cluster = %u\n", fat32_meta.sectors_per_cluster);
  printk("reserved_sector_nums = %u\n", fat32_meta.reserved_sector_nums);
  printk("file_allocation_table_nums = %u\n", fat32_meta.file_allocation_table_nums);
  printk("root directory cluster number = %u\n", rootdir_cluster_num);
  printk("sectors_per_fat = %u\n", fat32_meta.sectors_per_fat);

  printk("Data region offset: %#x\n",
      fat32_meta.reserved_sector_nums * fat32_meta.bytes_per_sector +
      fat32_meta.file_allocation_table_nums * fat32_meta.sectors_per_fat * fat32_meta.bytes_per_sector);

  struct vnode *vn = (struct vnode *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
  vn->mount = mount;
  vn->f_ops = &fat32_file_operations;
  vn->v_ops = &fat32_vnode_operations;
  vn->type = VNODE_TYPE_DIR;
  struct fat32_dirent *dirent = (struct fat32_dirent *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
  dirent->cluster_num = rootdir_cluster_num;
  dirent->vn = vn;
  vn->internal = dirent;

  mount->fs = fs;
  mount->root = vn;
}

int fat32_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct fat32_dirent *dirent = dir_node->internal;
  uint8_t buf[BLOCK_SIZE];
  read_data_region(dirent->cluster_num, buf);

  for (uint8_t *cur = buf; *cur != '\0'; cur += 32) {
    char name[13];
    gen_filename(cur, cur + 8, name);
    if (strcmp(name, component_name)) {
      continue;
    }

    struct vnode *vn = (struct vnode *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);
    struct fat32_dirent *dirent = (struct fat32_dirent *)PFN_TO_KVIRT(buddy_alloc(0)->pfn);

    dirent->vn = vn;
    dirent->attrs = cur[0xb];
    dirent->cluster_num = *((uint16_t *)(cur + 0x1a)) + (((uint32_t)*((uint16_t *)(cur + 0x14))) << 16);
    dirent->size = *((uint32_t *)(cur + 0x1c));
    dirent->parent_cluster_num = ((struct fat32_dirent *)(dir_node->internal))->cluster_num;
    strcpy(dirent->name, name);

    vn->f_ops = dir_node->f_ops;
    vn->internal = dirent;
    vn->mount = dir_node->mount;
    vn->type = (dirent->attrs & 0x10) ? VNODE_TYPE_DIR : VNODE_TYPE_FILE;
    vn->v_ops = dir_node->v_ops;
    *target = vn;
    return 0;
  }
  *target = NULL;
  return -1;
}

int fat32_read(struct file* file, void* buf, size_t len) {
  struct fat32_dirent *dirent = file->vnode->internal;
  int nread = 0, pos = file->f_pos;

  if (pos >= dirent->size) {
    return -1;
  }

  int level = pos / 512;
  uint32_t cluster_num = dirent->cluster_num;
  for (int l = 0; l < level; ++l) {
    cluster_num = get_fat_entry(cluster_num);
  }

  int iter = pos % 512;
  uint8_t content[512];
  read_data_region(cluster_num, content);
  for (; pos < dirent->size && nread < len; ++pos, ++nread) {
    ((uint8_t *)buf)[nread] = content[iter];
    if (++iter % 512 == 0) {
      iter = 0;
      cluster_num = get_fat_entry(cluster_num);
      read_data_region(cluster_num, content);
    }
  }

  file->f_pos = pos;
  return nread;
}

int fat32_write(struct file* file, const void* buf, size_t len) {
  struct fat32_dirent *dirent = file->vnode->internal;
  int nwritten = 0, pos = file->f_pos;

  int level = pos / 512;
  uint32_t cluster_num = dirent->cluster_num;
  for (int l = 0; l < level; ++l) {
    cluster_num = get_fat_entry(cluster_num);
  }

  /* If the file is empty, allocate a cluster for it */
  if (dirent->size == 0) {
    cluster_num = get_unused_cluster();
    update_file_cluster_num(dirent->parent_cluster_num, dirent->name, cluster_num);
    dirent->cluster_num = cluster_num;
  }

  int iter = pos % 512;
  uint8_t content[512];
  read_data_region(cluster_num, content);
  for (; nwritten < len; ++pos, ++nwritten) {
    content[iter] = ((uint8_t *)buf)[nwritten];
    if (++iter % 512 == 0) {
      write_data_region(cluster_num, content);
      iter = 0;
      cluster_num = get_fat_entry(cluster_num);
      if (cluster_num == EOC) {
        uint32_t new_cluster_num = get_unused_cluster();
        set_fat_entry(cluster_num, new_cluster_num);
        cluster_num = new_cluster_num;
      }
      read_data_region(cluster_num, content);
    }
  }
  write_data_region(cluster_num, content);

  if (pos > dirent->size) {
    update_file_size(dirent->parent_cluster_num, dirent->name, pos);
    dirent->size = pos;
  }

  file->f_pos = pos;
  return nwritten;
}

void gen_filename(const char *sfn, const char *sfe, char *filename) {
  int i = 0;
  for (; sfn[i] != ' ' && i < 8; ++i) {
    filename[i] = sfn[i];
  }
  if (*sfe == ' ') {
    filename[i] = '\0';
    return;
  }

  filename[i++] = '.';
  for (int j = 0; sfe[j] != ' ' && j < 3; ++i, ++j) {
    filename[i] = sfe[j];
  }
  filename[i] = '\0';
}

uint32_t get_fat_entry(uint32_t cluster_num) {
  uint32_t fat[128];
  read_partition_block(fat32_meta.reserved_sector_nums + (cluster_num / (512 / 4)), (uint8_t *)fat);
  return fat[cluster_num % (512 / 4)] & 0x0fffffff;
}

void set_fat_entry(uint32_t cluster_num, uint32_t val) {
  uint32_t fat[128];
  read_partition_block(fat32_meta.reserved_sector_nums + (cluster_num / (512 / 4)), (uint8_t *)fat);
  fat[cluster_num % (512 / 4)] = val;
  write_partition_block(fat32_meta.reserved_sector_nums + (cluster_num / (512 / 4)), (uint8_t *)fat);
}

void read_data_region(uint32_t cluster_num, char *buf) {
  read_partition_block(
      fat32_meta.reserved_sector_nums +
      fat32_meta.file_allocation_table_nums * fat32_meta.sectors_per_fat +
      (cluster_num - 2),
      buf
  );
}

void write_data_region(uint32_t cluster_num, char *buf) {
  write_partition_block(
      fat32_meta.reserved_sector_nums +
      fat32_meta.file_allocation_table_nums * fat32_meta.sectors_per_fat +
      (cluster_num - 2),
      buf
  );
}

uint32_t get_unused_cluster(void) {
  for (int cluster_num = 0; ; ++cluster_num) {
    if (get_fat_entry(cluster_num) == 0) {
      set_fat_entry(cluster_num, EOC);
      return cluster_num;
    }
  }
}

void update_file_cluster_num(uint32_t dir_cluster_num, const char *filename, uint32_t cluster_num) {
  uint8_t buf[512];
  read_data_region(dir_cluster_num, buf);
  for (uint8_t *cur = buf; *cur != '\0'; cur += 32) {
    char name[13];
    gen_filename(cur, cur + 8, name);
    if (!strcmp(name, filename)) {
      *((uint32_t *)(cur + 0x14)) = (cluster_num >> 16) & 0xffff;
      *((uint32_t *)(cur + 0x1a)) = cluster_num & 0xffff;
      write_data_region(dir_cluster_num, buf);
      return;
    }
  }
}

void update_file_size(uint32_t dir_cluster_num, const char *filename, size_t size) {
  uint8_t buf[512];
  read_data_region(dir_cluster_num, buf);
  for (uint8_t *cur = buf; *cur != '\0'; cur += 32) {
    char name[13];
    gen_filename(cur, cur + 8, name);
    if (!strcmp(name, filename)) {
      *((uint32_t *)(cur + 0x1c)) = size;
      write_data_region(dir_cluster_num, buf);
      return;
    }
  }
}


