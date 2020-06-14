#ifndef SDHOST_H
#define SDHOST_H

void sd_init ( );
void readblock ( int block_idx, void * buf );
void writeblock ( int block_idx, void * buf );

#endif