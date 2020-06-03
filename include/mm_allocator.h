#ifndef _MM_ALLOCATOR_H
#define _MM_ALLOCATOR_H
#include "uart.h"

#define MEMORY_SIZE 1024 * 1024 // 1Gb
#define PAGE_SIZE 4 // 4Kb
#define MAXMIMUM_LEVEL 19
#define ADDR_START 0x0
#define NULL 0x0
#define ADDR_END 1024 * 1024 - 1

typedef enum {
    FREE,
    USED,
} STATUS;

struct block {
  unsigned long addr_start;
  unsigned long addr_end;
  struct block* next;
  STATUS status;

};

struct buddy {
  struct block* block_list[MAXMIMUM_LEVEL];
};

extern struct buddy buddy_system;
extern struct block block_pool[4096];
void init_buddy_system();
struct block* get_space(unsigned long bytes);
int get_free_level(unsigned long bytes);
struct block* get_free_descriptor();
void free_space(struct block* free_block);
void print_buddy();

#endif
