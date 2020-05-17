#ifndef MMAP
#define MMAP

#include <stddef.h>
#define MAP_FIXED 1
#define MAP_ANONYMOUS 2
#define MAP_POPULATE 4
#define PROT_NONE 1
#define PROT_READ 2
#define PROT_WRITE 4
#define PROT_EXEC 8

void *do_mmap (void *addr, size_t len, int prot, int flags, void *file_start,
	       int file_offset);
void *sys_mmap (void *addr, size_t len, int prot, int flags, void *file_start,
		int file_offset);

#endif /* ifndef MMAP */
