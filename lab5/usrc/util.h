#ifndef __UTIL_H__
#define __UTIL_H__

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

#endif
