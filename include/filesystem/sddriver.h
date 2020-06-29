#ifndef _SD_H
#define _SD_H

#define BLOCK_SIZE 512

void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void sd_init();

#endif