#include "libc.h"

extern void uart_puts(char *s);
extern void uart_send(unsigned int c);
extern void halt();

char *itoa(int val, int base) {
  static char buf[32] = {0};
  int i = 30;
  if (val == 0) {
    buf[i] = '0';
    return &buf[i];
  }

  for (; val && i; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
  return &buf[i + 1];
}

int sstrcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *p = (unsigned char *)s;

  while (n--)
    *p++ = (unsigned char)c;

  return s;
}

void memcpy(void *dest, const void *src, size_t num) {
  uint8_t *dest8 = (uint8_t *)dest;
  const uint8_t *src8 = (const uint8_t *)src;
  while (num--) {
    *dest8++ = *src8++;
  }
}

void  __attribute__((noreturn)) abort() {
  uart_puts("System aborted\r\n");
  halt();
}

void uart_print(char *format, ...) {
  unsigned int i;
  unsigned int n;
  char *s;

  va_list arg;
  va_start(arg, format);

  for (char *traverse = format; *traverse != 0; traverse++) {
    while (*traverse != '%' && *traverse != 0) {
      uart_send(*traverse++);
    }

    if (*traverse == 0) break;

    /* move to the hole */
    traverse++;
    /* actions */
    switch (*traverse) {
    case 'c':
      i = va_arg(arg, int);
      uart_send(i);
      break;
    case 'd':
      i = va_arg(arg, int);
      if (i < 0) {
        i = -i;
        uart_send('-');
      }
      uart_puts(itoa(i, 10));
      break;
    case 'x':
      i = va_arg(arg, unsigned int);
      for (int c = 28; c >= 0; c-= 4) {
        /* get highest tetrad */
        n = (i >> c) & 0xF;
        /* 0-9 => '0'-'9', 10-15 => 'A'-'F' */
        n += (n > 9)? 0x37:0x30;
        uart_send(n);
      }
      break;
    case 's':
      s = va_arg(arg, char*);
      uart_puts(s);
      break;
    default:
      uart_send('%');
      uart_send(*traverse);
      break;
    }
  }

  va_end(arg);
}


void uart_println(char *format, ...) {
  uart_print(format);
  uart_puts("\r\n");
}
