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

#define bset(A,k) ( (A)[(k)/8] |=  (1 << ((k)%8)) )
#define bclr(A,k) ( (A)[(k)/8] &= ~(1 << ((k)%8)) )
#define btst(A,k) ( (A)[(k)/8] &   (1 << ((k)%8)) )

#define overlap(addr, size, pos, len) \
  (addr <= pos && (addr) + (size) > pos) || (pos <= addr && (pos) + (len) > addr)
#define overlap_end(beg, end, _beg, _end)                                          \
  ((beg <= _end && _beg < end) || (end >= _beg && beg < _end))


unsigned long pow2roundup(unsigned long x);

int *show_sp();

void show_addr(unsigned long addr);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

#ifdef RUN_ON_EL0
#define EL 0
#else
#define EL 1
#endif

#define xstr(s) str(s)
#define str(s) #s

#define assert(expr) if(!(expr)){ printfmt(xstr(expr) " assertion failed!"); }

#endif
