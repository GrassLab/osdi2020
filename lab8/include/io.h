#ifndef __IO_H__
#define __IO_H__

#include "map.h"
#include "sprintf.h"
#include "uart.h"

#define NEWLINE "\r\n"

#define __print_as_number(type)                                                \
  void __print_##type##_as_number(type);                                       \
  void _print_##type##_as_number(type, int);

#define UL unsigned long
#define ULL unsigned long long
#define __print_as_number_types char, int, long
#define extra_number_printing                                                  \
  long unsigned int : __print_UL_as_number,                                    \
                      unsigned long long : __print_ULL_as_number

#define define__print_as_number_types()                                        \
  MAP(__print_as_number, __print_as_number_types)                              \
  __print_as_number(UL) __print_as_number(ULL)

define__print_as_number_types();

#define LABAL(x)                                                               \
  x:                                                                           \
  __print_##x##_as_number
#define print_as_number(x)                                                     \
  _Generic((x), MAP_LIST(LABAL, __print_as_number_types),                      \
           extra_number_printing)(x)

#define getchar() uart_recv()
#define putchar(x) uart_send(x)
#define flush() uart_flush()

#define __print(x)                                                             \
  _Generic((x), char: uart_send, char*: uart_puts, \
            long: __print_long_as_number, int: __print_int_as_number, \
            extra_number_printing, default: __print_ULL_as_number \
            )(x);

#define print(...)                                                             \
  do {                                                                         \
    MAP(__print, __VA_ARGS__)                                                  \
  } while (0)

#define println(...) print(__VA_ARGS__, NEWLINE)
#define puts println
#define printfmt(fmt, ...) printf(fmt NEWLINE, ## __VA_ARGS__)

int expect(char *s);
unsigned long long get_nature(char *, int, int);

void print_ident(int indent);

#endif
