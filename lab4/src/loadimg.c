#ifndef WITHOUT_LOADER

#include "io.h"
#include "util.h"

extern unsigned char _kbeg;
extern unsigned char _kend;
extern unsigned char _bootloader_beg;

#define MEMTOP ((char *)0x40000000)
#define overlap(beg, end, _beg, _end)                                          \
  ((beg <= _end && _beg < end) || (end >= _beg && beg < _end))

char *nkbeg, *nkend;
unsigned long nksize;

void __attribute__((__section__(".bootloader"))) copy_and_load() {
  puts("done");
  char *ptr = nkbeg;
  printf("load to 0x%x" NEWLINE, nkbeg);
  printf(NEWLINE "load %d bytes @0x%x" NEWLINE, nksize, (ULL)ptr);
  print("please input image now...");
  while (nksize--) {
    char c = getchar();
    *ptr++ = c;
  }
  puts("done");
  printf("jump to 0x%x" NEWLINE, nkbeg);
  __asm__ volatile("br %0" ::"r"(nkbeg));
  puts("jump failed");
}

char *schedule_tmp_kernel(char *okbeg, char *okend, char *nkbeg, char *nkend) {
  char *tkbeg;
  unsigned long oksize = okend - okbeg;
  for (tkbeg = (char *)0x80000;
       tkbeg + oksize < MEMTOP && tkbeg < tkbeg + oksize; tkbeg += 0x10000) {
    if (!overlap(tkbeg, tkbeg + oksize, okbeg, okend) &&
        !overlap(tkbeg, tkbeg + oksize, nkbeg, nkend)) {
      return tkbeg;
    }
  }
  return 0x0;
}

void loadimg() {

  print("input size[@loc]: ");

  char addr_mode;
  nksize = get_nature(&addr_mode, 10, 1);

  if (addr_mode == '@')
    nkbeg = (char *)get_nature(0, 16, 1);
  else
    nkbeg = (char *)&_kend;

  puts("");

  nkend = nkbeg + nksize;

#ifndef NO_RELOC_SELF

  /* copy the current kernel */
  char *tkbeg, *okbeg = (char *)&_kbeg, *okend = (char *)&_kend,
               *bootloader_beg = (char *)&_bootloader_beg;

  if (overlap(okbeg, okend, nkbeg, nkend)) {

    /* move self kernel code to temp location */
    if ((tkbeg = schedule_tmp_kernel(okbeg, okend, nkbeg, nkend))) {
      char *okptr = okbeg, *tkptr = tkbeg;

      print("move old kernel code...");
      while (okptr != okend) {
        *tkptr = *okptr;
        tkptr++, okptr++;
      }
      puts("done");

      char *addr = bootloader_beg - okbeg + tkbeg + 8;
      // 8 bytes for skip function stack pop instruction
      printf("ok 0x%x - 0x%x" NEWLINE, okbeg, okend);
      printf("tk 0x%x - 0x%x" NEWLINE, tkbeg, tkbeg + (okend - okbeg));
      printf("nk 0x%x - 0x%x" NEWLINE, nkbeg, nkend);
      printf("old loader @ 0x%x" NEWLINE, bootloader_beg);
      printf("jump to 0x%x" NEWLINE, addr);
      print("jump to new location of old kernel...");
      __asm__ volatile("br %0" ::"r"(addr));
    }
  } else {
#endif
    char *ptr = nkbeg;
    printf("load to 0x%x" NEWLINE, nkbeg);
    printf(NEWLINE "load %d bytes @0x%x" NEWLINE, nksize, (ULL)ptr);
    print("please input image now...");
    while (nksize--) {
      char c = getchar();
      *ptr++ = c;
    }
    puts("done");
    printf("jump to 0x%x" NEWLINE, nkbeg);
    __asm__ volatile("br %0" ::"r"(nkbeg));
    puts("jump failed");
#ifndef NO_RELOC_SELF
  }
  printf("cannot load new kernel @0x%x "
         "becuase of no proper relocation for old kernel." NEWLINE,
         nkbeg);
#endif
}

#endif
