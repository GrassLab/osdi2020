#pragma once

#include "vfs.h"
#include <stdint.h>

/* sector information */
struct boot_sector {
  char ignore1[0xd];
  uint8_t sector_per_clus;    // 0xd
  uint16_t count_of_reserved; // 0xe
  uint8_t num_of_fat;         // 0x10
  char ignore2[0x13];
  uint32_t sectors_per_fat; // 0x24
  char ignore3[4];
  uint32_t root_dir_cluster; // 0x2c
} __attribute__((packed));

struct partition_entry {
  char ignore[0x1c6];
  uint32_t lba;
} __attribute__((packed));

struct directory_entry {
  char name[8];        // 0x00 Name
  char extension[3];   // 0x08 Ext
  char ignore1[9];     // 0x0b Attrs
  uint16_t cluster_hi; // 0x14 (2 byte)
  char ignore2[4];     // 0x16
  uint16_t cluster_lo; // 0x1a (2 byte)
  uint32_t size;       // 0x1c
} __attribute__((packed));

struct fat32_node {
  struct partition_entry *p;
  struct boot_sector *b;
  uint32_t cluster_index;
  uint32_t dir_entry;
  uint32_t dir_index;
};

struct filesystem *create_fat32();

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof(unsigned int))
#define EOC 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof(struct directory_entry))
