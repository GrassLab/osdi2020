#include "fat.h"
#include "libc.h"
#include "mm.h"
#include "sd.h"
#include "slab.h"
#include "vfs.h"

struct fat32_node {
  unsigned int cluster_index;
  char dir_entry;
  char dir_index;
  struct partition_entry *p;
  struct boot_sector *b;

} ;

unsigned long fat_read_member(char *f, unsigned int s) {
  unsigned long ret = 0;
  for (int i = 0; i < s; ++i, f++) {
    ret += *f << i * 8;
  }
  return ret;
}

static unsigned int get_data_region_offset(unsigned int index,
                                           struct fat32_node *node) {
  unsigned int offset = (node->p->lba + node->b->count_of_reserved +
                         node->b->num_of_fat * node->b->sectors_per_fat);

  /* return offset - node->b->root_dir_cluster; */
  return (index - node->b->root_dir_cluster) * node->b->sector_per_clus +
         offset;
}

static int filename_cmp(struct directory_entry *dir, const char *s) {
  char *name = dir->name;

  /* check name */
  for (int i = 0; i < 8; ++i) {
    if (name[i] != '\x20') {
      /* compare with the s */
      if (s[i] != name[i])
        return 1;
    }
  }

  return 0;
}

int fat_lookup(struct vnode *dir_node, struct vnode **target,
               const char *component_name);

/* find the next cluster */
static unsigned int cluster_next_index(struct fat32_node *node,
                                       unsigned int index) {
  unsigned int chains[CHAIN_LEN];
  unsigned int offset =
      node->p->lba + node->b->count_of_reserved + (index / CHAIN_LEN);

  readblock(offset, chains);
  return chains[index % CHAIN_LEN];
}

static unsigned int reterive_dir_entry(struct fat32_node *node,
                                       struct directory_entry *target) {
  /* 512Bytes directory entries */
  struct directory_entry dirs[DIR_LEN];

  /* check cluster boundary */
  if (node->dir_index >= DIR_LEN)
    return 1;
  if (node->cluster_index == node->b->root_dir_cluster) {
    target->size = 0;
    return 0;
  }

  /* read the 512byte from data region into dirs */
  readblock(get_data_region_offset(node->dir_entry, node), dirs);
  *target = dirs[node->dir_index];
  return 0;
}

int fat_write(struct file *file, const void *buf, size_t len) {
  char data[BLOCK_SIZE];

  struct fat32_node *node = file->vnode->internal;
  struct directory_entry dir;
  if (reterive_dir_entry(node, &dir) || dir.size == 0)
    return -1;

  len = len > dir.size - file->f_pos ? dir.size - file->f_pos : len;

  unsigned int index = node->cluster_index;
  unsigned int offset = get_data_region_offset(index, node);

  readblock(offset, data);
  memcpy(&data, &buf[file->f_pos], len);
  writeblock(offset, data);

  file->f_pos += len;
  return len;
}

int fat_read(struct file *file, void *buf, size_t len) {
  char data[BLOCK_SIZE];
  struct fat32_node *node = file->vnode->internal;
  struct directory_entry dir;
  if (reterive_dir_entry(node, &dir) || dir.size == 0)
    return -1;

  len = len > dir.size - file->f_pos ? dir.size - file->f_pos : len;

  readblock(get_data_region_offset(node->cluster_index, node), data);
  memcpy(buf, &data[file->f_pos], len);

  file->f_pos += len;
  return len;
}

int fat_create(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {}

struct vnode_operations _v_ops = {.create = fat_create, .lookup = fat_lookup};
struct file_operations _f_ops = {.read = fat_read, .write = fat_write};

struct vnode_operations *v_ops = &_v_ops;
struct file_operations *f_ops = &_f_ops;

static struct fat32_node *create_internal_node(struct fat32_node *parent,
                                               unsigned int cluster_index,
                                               unsigned int dir_entry,
                                               unsigned int dir_index) {

  struct fat32_node *new = kalloc(sizeof(struct fat32_node));
  new->p = parent->p;
  new->b = parent->b;
  new->cluster_index = cluster_index;
  new->dir_entry = dir_entry;
  new->dir_index = dir_index;

  return new;
}

int fat_lookup(struct vnode *dir_node, struct vnode **target,
               const char *component_name) {
  uart_println("[fat] lookup pathanme %s", component_name);

  char *d = kalloc(512);
  struct directory_entry dirs[DIR_LEN];
  struct fat32_node *node = dir_node->internal;

  /* tarversal all block */
  for (unsigned int index = node->cluster_index; index & ~EOC;
       index = cluster_next_index(node, index)) {

    uart_println("[fat] cluster index start at %d", index);

    /* reterive dir entries */
    readblock(get_data_region_offset(index, node), d);
    readblock(get_data_region_offset(index, node), dirs);


    /* search */
    for (int i = 0; i < DIR_LEN; ++i) {
      dirs[i].name = (char *)(d + i * 32);
      dirs[i].cluster_hi = fat_read_member(d + i * 32 + 0x14, 2);
      dirs[i].cluster_lo = fat_read_member(d + i * 32 + 0x1a, 2);
      dirs[i].size = fat_read_member(d + i * 32 + 0x1c, 4);

      uart_println("[fat under current dir] %s", dirs[i].name);

      if (filename_cmp(&dirs[i], component_name) == 0) {

        /* create a vnode */
        struct vnode *v = kalloc(sizeof(struct vnode));
        v->v_ops = v_ops;
        v->f_ops = f_ops;
        v->mount = dir_node->mount;
        v->internal = create_internal_node(
            node, (dirs[i].cluster_hi << 16) + dirs[i].cluster_lo, index, i);

        /* return the target */
        *target = v;
        uart_println("[fat] found target %s", dirs[i].name);

        return 0;
      }
    }
  }
  return -1;
}

struct partition_entry *parts[10];
struct boot_sector *bootscs[10];

char *fat_getpartition() {
  char *p = kalloc(512);
  readblock(0, p);
  return p;
}

char  *fat_getbootsector(unsigned int lba) {
  char *b = kalloc(512);
  readblock(lba, b);
  return b;
}


static int setup_mount(struct filesystem *fs, struct mount *mount) {
  char *p = fat_getpartition();
  struct partition_entry *parten = kalloc(sizeof(struct partition_entry));
  parten->lba = fat_read_member(p + 0x1c6, 4);
  uart_println("lba: %d", parten->lba);

  /* uart_println("%x, %x", p+0x1c6, &parten->lba); */

  char *b = fat_getbootsector(parten->lba);
  struct boot_sector *bootsc = kalloc(sizeof(struct boot_sector));
  bootsc->sector_per_clus   = fat_read_member(b + 0xd,  1);
  bootsc->count_of_reserved = fat_read_member(b + 0xe,  2);
  bootsc->num_of_fat        = fat_read_member(b + 0x10, 1);
  bootsc->sectors_per_fat   = fat_read_member(b + 0x24, 4);
  bootsc->root_dir_cluster  = fat_read_member(b + 0x2c, 4);


  uart_println("sector per cluster: %d", bootsc->sector_per_clus);
  uart_println("count of reserved : %d", bootsc->count_of_reserved);
  uart_println("num of fat        : %d", bootsc->num_of_fat);
  uart_println("sectors per fat   : %d", bootsc->sectors_per_fat);
  uart_println("root dir cluster  : %d", bootsc->root_dir_cluster);
  /* struct partition_entry *parten = fat_getpartition(); */

  struct fat32_node init_node; /* info and cluster index */
  init_node.p = parten;
  init_node.b = bootsc;

  struct vnode *v = kalloc(sizeof(struct vnode));

  v->v_ops = v_ops;
  v->f_ops = f_ops;
  v->count = 1;
  v->next = 0;
  v->sub_dir = 0;
  v->parent = v;
  v->is_dir = 1;
  v->basename = "/";



  struct fat32_node *new =  kalloc(sizeof(struct fat32_node));
  new->p = parten;
  new->b = bootsc;
  new->cluster_index = bootsc->root_dir_cluster;

  uart_println("%x %x", &new->dir_entry, &new->dir_index);

  new->dir_entry = 0;
  new->dir_index = 0;

  v->internal = new;
  /* v->internal = */
  /*     create_internal_node(&init_node, init_node.b->root_dir_cluster, 0, 0); */

  /* setup mount */
  mount->fs = fs;
  mount->root = v;
  cur_root = v;

  return 0;
}

struct filesystem *create_fat32() {
  struct filesystem *fs = kalloc(sizeof(struct filesystem));
  fs->name = "fat32";
  fs->setup_mount = setup_mount;
  return fs;
}

/* fat_begin_lba = Partition_LBA_Begin + Number_of_Reserved_Sectors; */
/* cluster_begin_lba = Partition_LBA_Begin + Number_of_Reserved_Sectors +
 * (Number_of_FATs * Sectors_Per_FAT); */
/* sectors_per_cluster = BPB_SecPerClus; */
/* root_dir_first_cluster = BPB_RootClus; */
