#include <stdarg.h>
#include "kernel/mini_uart.h"
#include "kernel/syscall.h"
#include "kernel/lib/string.h"
#include "kernel/lib/types.h"
#include "kernel/lib/utils.h"

int printk(const char *format, ...) {
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
            do_uart_write("[ERROR] Invalid format specifier\n", strlen("[ERROR] Invalid format specifier\n"));
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
  do_uart_write(buf, strlen(buf));
  return strlen(buf);
}
