#ifndef _SD_H
#define _SD_H

// SD card command
#define GO_IDLE_STATE 0
#define SEND_OP_CMD 1
#define ALL_SEND_CID 2
#define SEND_RELATIVE_ADDR 3
#define SELECT_CARD 7
#define SEND_IF_COND 8
#define VOLTAGE_CHECK_PATTERN 0x1aa
#define STOP_TRANSMISSION 12
#define SET_BLOCKLEN 16
#define READ_SINGLE_BLOCK 17
#define WRITE_SINGLE_BLOCK 24
#define SD_APP_OP_COND 41
#define SDCARD_3_3V (1 << 21)
#define SDCARD_ISHCS (1 << 30)
#define SDCARD_READY (1 << 31)
#define APP_CMD 55

// helper
#define set(io_addr, val) \
  asm volatile("str %w1, [%0]" ::"r"(io_addr), "r"(val) : "memory");

#define get(io_addr, val) \
  asm volatile("ldr %w0, [%1]" : "=r"(val) : "r"(io_addr) : "memory");


void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void sd_init();

#endif
