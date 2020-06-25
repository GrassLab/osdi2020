#include <stdint.h>
#ifndef __MBR_H__
#define __MBR_H__

#define MBR_FIRST_PARTITION_ENTRY 446
#define MBR_PARTITION_ENTRY_SIZE 16

#define MBR_HPC 16u
#define MBR_SPT 63u

struct mbr_partition_entry_struct
{
  uint16_t start_c; /* 10 bits */
  uint8_t start_h; /* 8 bits */
  uint8_t start_s; /* 6 bits */ /* in partition entry the msb two bit of sector is for the msb two bit for cylinder */
  uint8_t partition_type;
  uint16_t end_c;
  uint8_t end_h;
  uint8_t end_s;
  uint32_t lba_start;
  uint32_t total_sectors;
};

void mbr_get_primary_partition(struct mbr_partition_entry_struct * buf, unsigned idx);

uint32_t mbr_chs_to_lba(uint16_t c, uint8_t h, uint8_t s);

#endif

