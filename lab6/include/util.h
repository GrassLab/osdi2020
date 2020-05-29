#ifndef __UTIL_H__
#define __UTIL_H__

//extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);
extern void set_pgd(unsigned long pgd);

#ifdef ON_QEMU
#define delay(t)                                                               \
  do {                                                                         \
    int r = t * 100;                                                                 \
    while (r--) {                                                              \
      __asm__ volatile("nop");                                                 \
    }                                                                          \
  } while (0)
#else
#define delay(t)                                                               \
  do {                                                                         \
    int r = t;                                                                 \
    while (r--) {                                                              \
      __asm__ volatile("nop");                                                 \
    }                                                                          \
  } while (0)
#endif

#define DELAY(t)                                                               \
  do {                                                                         \
    int r = t;                                                                 \
    while (r--) {                                                              \
      __asm__ volatile("nop");                                                 \
    }                                                                          \
  } while (0)

#define sizeofType(type) printfmt("size of " #type " is %d", sizeof(type))

#endif
