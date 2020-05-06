#ifndef __MM_H__
#define __MM_H__

#define KERNEL_BASE     0x80000

#define PAGE_SIZE       4096

#define KSTK_SIZE       PAGE_SIZE
#define USTK_SIZE       PAGE_SIZE

#endif

#ifndef IN_ASM // asm may also include this header file

extern char* kstack_pool[];
extern char* ustack_pool[];
extern int kstack_avaliable[];
extern int ustack_avaliable[];

void mm_init();
char* get_avaliable_ustack();
char* get_avaliable_kstack();

#endif
