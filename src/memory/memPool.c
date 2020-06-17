#include "type.h"
#include "memory/memManager.h"
#include "memory/buddy.h"
#include "memory/memPool.h"
#include "device/uart.h"

static const uint32_t MAX_POOLS = 41;
static const uint32_t MAX_FREE_POOLS = 32;
struct memPool* pool;
extern uint64_t used_mem;

uint64_t allocSlot(uint32_t token)
{
    if (pool[token].init_addr == 0)
    {
        pool[token].init_addr = allocFreePage(0);

        // uartPuts("allocate free page with init address: ");
        // uartHex(pool[token].init_addr);
        // uartPuts(" for pool ");
        // uartInt(token);
        // uartPuts("\n");
    }

    for (uint32_t i = 0, end = pool[token].max_slot_num; i < end; ++i)
    {
        if (pool[token].slot_used[i] == false)
        {
            pool[token].slot_used[i] = true;

            // uartPuts("allocate slot ");
            // uartInt(i);
            // uartPuts(" of pool ");
            // uartInt(token);
            // uartPuts("\n");

            return pool[token].init_addr + i * pool[token].slot_size;
        }
    }

    return 0;
}

int32_t getFreePool(uint64_t size)
{
    for (int32_t i = 0; i < MAX_FREE_POOLS; ++i)
    {
        if (pool[i].pool_used == false)
        {
            pool[i].slot_size = size;
            pool[i].max_slot_num = PAGE_SIZE / size;
            pool[i].pool_used = true;

            // uartPuts("allocate free pool with token ");
            // uartInt(i);
            // uartPuts("\nslot size: ");
            // uartInt(pool[i].slot_size);
            // uartPuts("\nmax slot number: ");
            // uartInt(pool[i].max_slot_num);
            // uartPuts("\n");

            return i;
        }
    }

    return -1;
}

void freeSlot(uint32_t token, uint64_t addr)
{
    uint32_t index = (addr - pool[token].init_addr) / pool[token].slot_size;

    // uartPuts("free slot ");
    // uartInt(index);
    // uartPuts(" of pool ");
    // uartInt(token);
    // uartPuts("\n");

    pool[token].slot_used[index] = false;
}

void freePool(uint32_t token)
{
    // uartPuts("free pool ");
    // uartInt(token);
    // uartPuts("\n");

    pool[token].pool_used = false;
    freePage(pool[token].init_addr);
}

uint64_t allocDynamic(uint64_t size)
{
    for (uint32_t i = MAX_FREE_POOLS; i < MAX_POOLS; ++i)
    {
        if (size < pool[i].slot_size)
        {
            uint64_t addr = allocSlot(i);
            if (addr > 0)
                return addr;
            else 
                continue;
        }
    }

    // If there's no free slot or the size is larger than 2048
    for (uint32_t i = 0; i <= MAX_ORDER; ++i)
    {
        if (size < PAGE_SIZE * (1 << i))
        {
            return allocFreePage(i);
        }
    }

    return 0;
}

void freeDynamic(uint64_t addr)
{
    for (uint32_t i = MAX_FREE_POOLS; i < MAX_POOLS; ++i)
    {
        if (addr >= pool[i].init_addr && addr < (pool[i].init_addr + PAGE_SIZE))
        {
            freeSlot(i, addr);
            return;
        }
    }

    freePage(addr);
}

void initMemPool()
{
    pool = (struct memPool* )(VA_START + LOW_MEMORY + used_mem); 
    used_mem += sizeof(struct memPool) * MAX_POOLS;

    for (uint32_t i = 0; i < MAX_FREE_POOLS; ++i)
    {
        pool[i].pool_used = false;
        pool[i].init_addr = 0;
        for (uint64_t j = 0; j < 4096; ++j)
            pool[i].slot_used[j] = false;
    }

    // Allocate memory pool from size 8 to size 2048
    for (uint32_t i = MAX_FREE_POOLS, j = 3; i < MAX_POOLS; ++i, ++j)
    {
        uint64_t size = 1 << j;
        pool[i].pool_used = true;
        pool[i].slot_size = size;
        pool[i].max_slot_num = PAGE_SIZE / size;
        pool[i].init_addr = allocFreePage(0);
        for (uint64_t j = 0; j < 4096; ++j)
            pool[i].slot_used[j] = false;

        // uartPuts("allocate free page with init address: ");
        // uartHex(pool[i].init_addr);
        // uartPuts(" for pool ");
        // uartInt(i);
        // uartPuts("\n");
    }
}