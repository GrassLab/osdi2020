#include "io.h"
#include "fs.h"
#include "string.h"
#include "allocator.h"

#undef __print_as_number
#define __print_as_number(type)                                                \
  void __print_##type##_as_number(type c) {                                    \
    if (c < 0)                                                                 \
      putchar('-');                                                            \
    _print_##type##_as_number(c < 0 ? -c : c, 10);                             \
  }                                                                            \
  void _print_##type##_as_number(type c, int base) {                           \
    if (c / base) {                                                            \
      _print_##type##_as_number(c / base, base);                               \
    }                                                                          \
    putchar("0123456789ABCDEF"[c % base]);                                     \
  }

define__print_as_number_types();

int expect(char *s) {
  char c;
  while (*s) {
    c = getchar();
    if (*s != c) {
      println("expect ", *s, "get", c);
      return 0;
    }
    s++;
  }
  return 1;
}

int is_digit(char c, int base) {
  if (c >= 97)
    c = c - 'a' + 'A';
  for (int i = 0; i < base; i++)
    if (c == "0123456789ABCDEFGHIJKLMNOPQRSTUVWXUZ"[i])
      return i;
  return -1;
}

unsigned long long get_nature(char *p, int base, int echo) {
  int n = 0, c, d;
  while ((d = is_digit(c = getchar(), base)) >= 0) {
    n = n * base + d;
    if (echo)
      putchar(c);
  }
  if (p)
    *p = c;
  if (echo)
    putchar(c);
  return n;
}

void print_ident(int indent){
  while(indent--) printf(" ");
}

void fprintf(FILE *fd, char *fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  char *buffer = (char*)kmalloc(256 * sizeof(char));
  if(fd){
    vsprintf(buffer, fmt, args);
    vfs_write(fd, buffer, strlen(buffer));
  } else vsprintf(0, fmt, args);
;
}
