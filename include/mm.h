#ifndef __MM_H__
#define __MM_H__

#define PAGE_SIZE       4096

#define KSTK_SIZE       PAGE_SIZE
#define USTK_SIZE       PAGE_SIZE

#endif

extern char* kstack_pool[];

void mm_init();
