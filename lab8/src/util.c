#include "io.h"

unsigned long
pow2roundup (unsigned long x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

int *show_sp(){
  int *sp;
  __asm__ volatile("mov %0, sp":"=r"(sp));
  printf("sp = 0x%x" NEWLINE, sp);
  return sp;
}

void show_addr(unsigned long addr){
  printfmt("addr = 0x%x", addr);
}

unsigned long read_le(char *p, int size){
  unsigned long ret = 0;
  for(int i = 0; i < size; i++, p++){
    ret += *p << i * 8;
  }
  return ret;
}

void write_le(char *p, unsigned long v, int size){
  while(size--){
    *p = v & 0xFF;
    p++, v >>= 8;
  }
}
