#include "type.h"
#include "memory/memManager.h"
#include "device/uart.h"

static const uint32_t MAX_POOLS = 32;
static struct memPool pool[32];

uint64_t allocSlot(uint32_t token)
{
    if (pool[token].init_addr == 0)
    {
        pool[token].init_addr = getFreePage(0);

        uartPuts("allocate free page with init address: ");
        uartHex(pool[token].init_addr);
        uartPuts(" for pool ");
        uartInt(token);
        uartPuts("\n");
    }

    for (uint32_t i = 0, end = pool[token].max_slot_num; i < end; ++i)
    {
        if (pool[token].slot_used[i] == false)
        {
            pool[token].slot_used[i] = true;

            uartPuts("allocate slot ");
            uartInt(i);
            uartPuts(" of pool ");
            uartInt(token);
            uartPuts("\n");

            return pool[token].init_addr + i * pool[token].slot_size;
        }
    }

    return 0;
}

int32_t getFreePool(uint64_t size)
{
    for (int32_t i = 0; i < MAX_POOLS; ++i)
    {
        if (pool[i].pool_used == false)
        {
            pool[i].slot_size = size;
            pool[i].max_slot_num = PAGE_SIZE / size;
            pool[i].pool_used = true;

            uartPuts("allocate free pool with token ");
            uartInt(i);
            uartPuts("\nslot size: ");
            uartInt(pool[i].slot_size);
            uartPuts("\nmax slot number: ");
            uartInt(pool[i].max_slot_num);
            uartPuts("\n");

            return i;
        }
    }

    return -1;
}

void freeSlot(uint32_t token, uint64_t addr)
{
    uint32_t index = (addr - pool[token].init_addr) / pool[token].slot_size;

    uartPuts("free slot ");
    uartInt(index);
    uartPuts(" of pool ");
    uartInt(token);
    uartPuts("\n");

    pool[token].slot_used[index] = false;
}

void initMemPool()
{
    for (uint32_t i = 0; i < MAX_POOLS; ++i)
    {
        pool[i].pool_used = false;
        pool[i].init_addr = 0;
        for (uint64_t j = 0; j < 4096; ++j)
            pool[i].slot_used[j] = false;
    }
}