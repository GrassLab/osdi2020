#ifndef __SDHOST_H__
#define __SDHOST_H__
void sd_init();
void readblock(int block_id, char buf[512]);
void writeblock(int block_id, char buf[512]);
#endif
