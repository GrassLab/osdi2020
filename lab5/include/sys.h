#ifndef __SYS_H__
#define __SYS_H__

#ifndef __ASSEMBLER__

int syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4, long x5);

#endif

#endif
