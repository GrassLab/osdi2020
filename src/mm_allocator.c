#include "mm_allocator.h"

struct buddy buddy_system;
struct block block_pool[4096];

void init_buddy_system() {
  for (int num = 0; num < 4096; num++)
    block_pool[num].status = FREE;
  struct block* max_block = get_free_descriptor();
  max_block->addr_start = ADDR_START;
  max_block->addr_end = ADDR_END;
  max_block->next = NULL;
  for (int num = 0; num < MAXMIMUM_LEVEL; num++)
    buddy_system.block_list[num] = NULL;
  buddy_system.block_list[MAXMIMUM_LEVEL - 1] = max_block;
  uart_puts("buddy system initialization!\n");
}

struct block* get_space(unsigned long bytes) {
  int level = get_free_level(bytes);
    uart_int(level);
    uart_puts("\n");
  int current_level = level;
  while (buddy_system.block_list[current_level] == NULL) {
    current_level++;
    if (current_level == MAXMIMUM_LEVEL)
      return NULL;
  }
  uart_int(current_level);
  uart_puts("\n");

  struct block* current_block = buddy_system.block_list[current_level];
  buddy_system.block_list[current_level] = buddy_system.block_list[current_level]->next;

  while (current_level > level) {
    // remain block append in next level
    struct block* new_block = get_free_descriptor();
    new_block->addr_start = (current_block->addr_end + current_block->addr_start) / 2 + 1;
    new_block->addr_end = current_block->addr_end;
    new_block->next = buddy_system.block_list[current_level - 1];
    buddy_system.block_list[current_level - 1] = new_block;
    current_block->addr_end = new_block->addr_start - 1;
    current_level--;
  }
  print_buddy();
  uart_puts("\n");
  uart_puts("free space from: ");
  uart_hex(current_block->addr_start);
  uart_puts(" to ");
  uart_hex(current_block->addr_end);
  uart_puts("\n");
  return current_block;
}

int get_free_level(unsigned long bytes) {
  int level;
  for (level = 0; level < MAXMIMUM_LEVEL; level++)
    if (bytes <= (1 << level << 12))
      break;
  return level;
}

struct block* get_free_descriptor() {
  for (int num = 0; num < 4096; num++)
    if (block_pool[num].status == FREE) {
      block_pool[num].status = USED;
      return &(block_pool[num]);
    }
  return NULL;
}

void free_space(struct block* free_block) {
  unsigned long size = free_block->addr_end - free_block->addr_start;
  int level = get_free_level(size << 10);
  int current_level = level;
  int flag = 1;
  struct block* new_block = get_free_descriptor();
  new_block->addr_start = free_block->addr_start;
  new_block->addr_end = free_block->addr_end;
  new_block->next = free_block->next;
  while (flag == 1) {
    flag = 0;
    struct block* current_block = buddy_system.block_list[current_level];
    struct block* previous_block = buddy_system.block_list[current_level];
    while (current_block != NULL) {
      if ((current_block->addr_start == new_block->addr_end + 1) ||
          (current_block->addr_end == new_block->addr_start - 1)) { // contiguous
        if (previous_block == current_block)
          buddy_system.block_list[current_level] = current_block->next;
        else
          previous_block->next = current_block->next;
        if (current_block->addr_start == new_block->addr_end + 1)
          new_block->addr_end = current_block->addr_end;
        if (current_block->addr_end == new_block->addr_start - 1)
          new_block->addr_start = current_block->addr_start;
        flag = 1;
        uart_puts("merge space from: ");
        uart_hex(new_block->addr_start);
        uart_puts(" to ");
        uart_hex(new_block->addr_end);
        uart_puts("\n");
        break;
      }
      if (previous_block != current_block)
        previous_block = previous_block->next;
      current_block = current_block->next;
    }
  current_level++;
  }
  current_level--;
  struct block* current_block = buddy_system.block_list[current_level];
  new_block->next = buddy_system.block_list[current_level];
  buddy_system.block_list[current_level] = new_block;

  print_buddy();
  uart_puts("\n");
  uart_puts("\n");
}


void print_buddy() {
  for (int num = 0; num < MAXMIMUM_LEVEL; num++) {
    uart_int(num);
    uart_puts(": ");
    struct block* pb = buddy_system.block_list[num];
    while (pb != NULL) {
      uart_puts(" |  ");
      uart_hex(pb->addr_start);
      uart_puts(" -> ");
      uart_hex(pb->addr_end);
      uart_puts("  | ");
      pb = pb->next;
    }
    uart_puts("\n");
  }
}

