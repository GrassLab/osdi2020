#ifndef __SD_HOST_H__
#define __SD_HOST_H__

void sd_init( void );
void readblock( int block_idx, void * buf );
void writeblock( int block_idx, void * buf );



#endif