#pragma once

#include "vfs.h"
#include <stdint.h>

/* sector information */
struct boot_sector {
  uint8_t sector_per_clus;    // 0xd
  uint16_t count_of_reserved; // 0xe
  uint8_t num_of_fat;         // 0x10
  uint32_t sectors_per_fat; // 0x24
  uint32_t root_dir_cluster; // 0x2c
};

struct partition_entry {
  uint32_t lba;
};

struct directory_entry {
  /* char name[8];        // 0x00 Name */
  char *name;
  char extension[3];   // 0x08 Ext
  char ignore1[9];     // 0x0b Attrs
  uint16_t cluster_hi; // 0x14 (2 byte)
  char ignore2[4];     // 0x16
  uint16_t cluster_lo; // 0x1a (2 byte)
  uint32_t size;       // 0x1c
};

struct fat32_node {
  unsigned int cluster_index;
  unsigned int dir_entry;
  unsigned int dir_index;
  struct partition_entry *p;
  struct boot_sector *b;
};

struct filesystem *create_fat32();

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof(unsigned int))
#define EOC 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof(struct directory_entry))
