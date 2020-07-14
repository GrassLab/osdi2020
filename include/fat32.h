#include "sd_driver.h"
#include "uart.h"

typedef struct boot_sector_t {
  char padding0[0xe];
  unsigned short count_of_reserved; // 0x00E
  unsigned char num_of_fat;         // 0x010
  char padding1[0x13];
  unsigned int sectors_per_fat; // 0x024
  char padding2[4];
  unsigned int cluster_num_of_root; // 0x02c
} boot_sector_t;

typedef struct fat32_metadata_t {
  unsigned int lba;
  unsigned int size;
  unsigned short count_of_reserved;
  unsigned char num_of_fat;
  unsigned int sectors_per_fat;
  unsigned int cluster_num_of_root;
  unsigned int offset;
} fat32_metadata_t;


void print_block(int block);
void get_fat32_partition();
void fat32_init();
