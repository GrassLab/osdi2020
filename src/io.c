#include "io.h"

#undef __print_as_number
#define __print_as_number(type) \
    void __print_ ## type ## _as_number(type c) \
        { _print_ ## type ## _as_number(c, 1); } \
    void _print_ ## type ## _as_number(type c, int top){ \
        if(c % 10){ \
            _print_ ## type ## _as_number(c / 10, 0); \
            putchar("0123456789"[c % 10]); \
        } \
        else if(top) putchar('0'); \
    }

define__print_as_number_types();
