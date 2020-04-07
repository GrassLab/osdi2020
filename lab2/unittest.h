#pragma once

#include "uart.h"

#define TEST_FAILED(name, description)                                         \
  { uart_println("\033[0;31m[TEST][ERR] %s: %s\033[0m", name, description); }

#define TEST_SUCCESS(name, description)                                        \
  { uart_println("\033[0;32m[TEST][OK] %s: %s\033[0m", name, description); }

#define TEST1(test, name, desc)                                                \
  {                                                                            \
    if (test() == 0) {                                                         \
      TEST_SUCCESS(name, desc);                                                \
    } else {                                                                   \
      TEST_FAILED(name, desc);                                                 \
    }                                                                          \
  }


void unittest(int (*f)(), char *s, char *de) {
  #ifdef TEST
  TEST1(f, s, de);
  #endif
}
