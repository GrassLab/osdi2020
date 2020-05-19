#include <stdarg.h>
#include "user/lib/syscall.h"
#include "user/lib/string.h"
#include "user/lib/types.h"
#include "user/lib/utils.h"

int printf(const char *format, ...) {
  va_list ap;
  char buf[256];
  va_start(ap, format);

  const char *iter = format;
  char *out = buf;
  while (*iter != '\0') {
    if (*iter == '%') {
      while (true) {
        ++iter;
        if (*iter == '#') {
          if (*(iter + 1) == 'x') {
            out += strlen(strcpy(out, "0x"));
          } else {
            --iter;
            break;
          }
        } else if (*iter == 'u') {
          out += strlen(uitos(va_arg(ap, uint64_t), out));
          break;
        } else if (*iter == 'x') {
          out += strlen(uitos_generic(va_arg(ap, uint64_t), 16, out));
          break;
        } else if (*iter == 's') {
          out += strlen(strcpy(out, va_arg(ap, char *)));
          break;
        } else {
            uart_write("[ERROR] Invalid format specifier\n", strlen("[ERROR] Invalid format specifier\n"));
          return -1;
        }
      }
      ++iter;
    } else {
      *out = *iter;
      ++iter;
      ++out;
    }
  }

  *out = '\0';
  uart_write(buf, strlen(buf));
  return strlen(buf);
}

char *gets(char *s) {
  for (char *cur = s; ;++cur) {
    if (uart_read(cur, 1) == 0 || *cur == '\n') {
      *cur = '\0';
      return s;
    }
  }
}

int puts(const char *s) {
  for (const char *cur = s; *s != '\0'; ++s) {
    if (uart_write(cur, 1) != 1) {
      return cur - s;
    }
  }
}