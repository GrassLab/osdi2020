#include "io.h"

#undef __print_as_number
#define __print_as_number(type) \
    void __print_ ## type ## _as_number(type c) \
        { if(c < 0) putchar('-'); _print_ ## type ## _as_number(c < 0 ? -c : c); } \
    void _print_ ## type ## _as_number(type c){ \
        if(c / 10){ \
            _print_ ## type ## _as_number(c / 10); \
        } \
        putchar("0123456789"[c % 10]); \
    }

define__print_as_number_types();
