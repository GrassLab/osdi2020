#include "mbr.h"
#include "sd.h"

void mbr_get_primary_partition(struct mbr_partition_entry_struct * buf, unsigned idx)
{
  /* idx should be < 4 */
  uint8_t block_buf[512];
  sd_readblock(0, block_buf);

  buf -> start_h = block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 1];
  buf -> start_s = block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 2] & 0x3f;
  buf -> start_c = (uint16_t)((block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 2] & 0xc0) << 2);
#pragma GCC diagnostic ignored "-Wconversion"
  buf -> start_c |= block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 3];

  buf -> partition_type = block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 4];

  buf -> end_h = block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 5];
  buf -> end_s = block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 6] & 0x3f;
  buf -> end_c = (uint16_t)((block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 6] & 0xc0) << 2);
#pragma GCC diagnostic ignored "-Wconversion"
  buf -> end_c |= block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 7];

  buf -> lba_start = *(uint32_t *)(&block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 0x8]);
  buf -> total_sectors = *(uint32_t *)(&block_buf[MBR_FIRST_PARTITION_ENTRY + idx * MBR_PARTITION_ENTRY_SIZE + 0xc]);
  return;
}

uint32_t mbr_chs_to_lba(uint16_t c, uint8_t h, uint8_t s)
{
  /* LBA = (C x HPC + H) x SPT + (S − 1) */
  return (c * MBR_HPC + h) * MBR_SPT + (s - 1u);
}

