#ifndef _MP_H
#define _MP_H

#include "type.h"

struct memPool
{
    uint64_t init_addr;
	uint64_t slot_size;
	uint32_t max_slot_num;
	bool pool_used;
	bool slot_used[4096]; 
};

uint64_t allocSlot(uint32_t token);
int32_t getFreePool(uint64_t size);
void freeSlot(uint32_t token, uint64_t addr);
void freePool(uint32_t token);
uint64_t allocDynamic(uint64_t size);
void freeDynamic(uint64_t addr);
void initMemPool();

#endif