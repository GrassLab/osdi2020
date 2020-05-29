#ifndef _MM_ALLOCATOR_H_
#define _MM_ALLOCATOR_H_

void init_buddy_system();
unsigned long get_free_space(unsigned long bytes);
void free_space(unsigned long address);

#endif