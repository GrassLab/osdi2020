#ifndef _MM_ALLOCATOR_H
#define _MM_ALLOCATOR_H
#include "uart.h"

#define MEMORY_SIZE 1024 * 1024 // 1Gb
#define PAGE_SIZE 4 // 4Kb
#define MAXMIMUM_LEVEL 19
#define ADDR_START 0x0
#define NULL 0x0
#define ADDR_END 1024 * 1024 - 1
#define MAX_BLOCK_NUM 4096
#define MAX_ALLOCATOR_NUM 4096
#define MAX_CHUNK_NUM 4096 * 4096

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

struct chunk {
  unsigned long addr;
  struct chunk* next;
  STATUS status;
};

struct obj_alloc {
  struct block* block_list;
  unsigned long obj_size;
  unsigned long id;
  struct chunk* chunk_list;
  STATUS status;
};

extern struct buddy buddy_system;
extern struct block block_pool[MAX_BLOCK_NUM];
extern struct obj_alloc allocator_pool[MAX_ALLOCATOR_NUM];
extern struct chunk chunk_pool[MAX_CHUNK_NUM];


void init_buddy_system();
struct block* get_space(unsigned long bytes);
int get_free_level(unsigned long bytes);
struct block* get_free_descriptor();
void free_space(struct block* free_block);
void print_buddy();

int register_obj_allocator(int size);
struct block* block_allocate(struct chunk** chunk_list, int size);
void* fixed_obj_allocate(int token);
struct obj_alloc* get_free_allocator();
struct chunk* get_free_chunk();
void print_chunk(struct chunk* chunk_list);

#endif
