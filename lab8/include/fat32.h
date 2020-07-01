#ifndef _FAT32_H_
#define _FAT32_H_

#include "stdint.h"

struct directory_entry {
	char name[8]; // 0x00
	char extension[3]; // 0x08
	char ignore1[9]; // 0x0b
	uint16_t start_hi; // 0x14
	char ignore2[4]; // 0x16
	uint16_t start_lo; // 0x1a
	uint32_t size; // 0x1c
} __attribute__((packed));

struct boot_sector {
	char ignore1[0xe];
	uint16_t count_of_reserved; //0xe
	uint8_t num_of_fat; // 0x10
	char ignore2[0x13];
	uint8_t sectors_per_fat; // 0x24
	char ignore3[4];
	uint32_t cluster_num_of_root; // 0x2c
} __attribute__((packed));

struct partition_entry {
	char ignore[0x1c6];
	uint32_t lba;
	uint32_t size;
} __attribute__((packed));

struct fat32_info {
	uint32_t lba;
	uint32_t size;
	uint16_t count_of_reserved;
	uint8_t num_of_fat;
	uint32_t sectors_per_fat;
	uint32_t cluster_num_of_root;
};

struct fat32_node {
	struct fat32_info info;
	uint32_t cluster_index;
	uint32_t dir_entry;
	uint32_t dir_index;
};

#endif