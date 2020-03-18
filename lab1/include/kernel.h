#ifndef _KERNEL_INCLUDE_H_
#define _KERNEL_INCLUDE_H_

#ifndef NULL
#define NULL (void *)0
#endif
void printk(const char *fmt, ...);

void print(const char *fmt, ...);

#endif