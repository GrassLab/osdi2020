#ifndef __MMU_KMALLOC_H
#define __MMU_KMALLOC_H

#include <stddef.h>

void kmalloc_init(void);
void kmalloc_fini(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
