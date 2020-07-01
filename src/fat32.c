#include "fat32.h"

static fat32_metadata_t fat32_meta;

void print_fat32_block(int block) {
  char buf[512];
  uart_puts("Partition 1's ");
  uart_int(block);
  uart_puts(" block: \n");
  readblock(block, &buf);

  for (int i = 0; i < 512; i += 16) {
    uart_addr(i + block * 512);
    uart_puts(": ");
    for (int j = 0; j < 16; j += 2) {
      uart_shex((buf[i + j]));
      uart_shex((buf[i + j + 1]));
      uart_send(' ');
    }
    uart_puts("\n\r");
  }
  uart_puts("============================\n\r");
}

void get_fat32_partition() {
  char but[512];
  int lba = 2048, size = 0;
  print_fat32_block(0);

  boot_sector_t b;
  readblock(lba, &b);
  print_fat32_block(lba);

  //fat32_memcpy((char *)&b, (char *)&buf, 512);

  uart_puts("count_of_reserved: ");
  uart_hex(b.count_of_reserved);
  uart_send('\n');
  uart_puts("num_of_fat: ");
  uart_hex(b.num_of_fat);
  uart_send('\n');
  uart_puts("sectors_per_fat: ");
  uart_hex(b.sectors_per_fat);
  uart_send('\n');
  uart_puts("cluster_num_of_root: ");
  uart_hex(b.cluster_num_of_root);
  uart_send('\n');

  print_fat32_block(b.count_of_reserved * b.cluster_num_of_root);
  int offset = lba + b.count_of_reserved + b.num_of_fat * b.sectors_per_fat;
  print_fat32_block(offset);

  fat32_meta.cluster_num_of_root = b.cluster_num_of_root;
  fat32_meta.num_of_fat = b.num_of_fat;
  fat32_meta.sectors_per_fat = b.sectors_per_fat;
  fat32_meta.count_of_reserved = b.count_of_reserved;
  fat32_meta.offset = offset;

  //fat32_root = b.cluster_num_of_root;
}

void fat32_init() {
  sd_init();
  get_fat32_partition();
}

