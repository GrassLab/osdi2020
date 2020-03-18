#include "io.h"

#undef __print_as_number
#define __print_as_number(type) \
    void __print_ ## type ## _as_number(type c) \
        { if(c < 0) putchar('-'); _print_ ## type ## _as_number(c < 0 ? -c : c, 10); } \
    void _print_ ## type ## _as_number(type c, int base){ \
        if(c / 10){ \
            _print_ ## type ## _as_number(c / 10, base); \
        } \
        putchar("0123456789ABCDEF"[c % 10]); \
    }

define__print_as_number_types();
