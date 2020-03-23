#include "io.h"

#undef __print_as_number
#define __print_as_number(type) \
    void __print_ ## type ## _as_number(type c) \
        { if(c < 0) putchar('-'); _print_ ## type ## _as_number(c < 0 ? -c : c, 10); } \
    void _print_ ## type ## _as_number(type c, int base){ \
        if(c / base){ \
            _print_ ## type ## _as_number(c / base, base); \
        } \
        putchar("0123456789ABCDEF"[c % base]); \
    }

define__print_as_number_types();

int expect(char* s){
    char c;
    while(*s){
        c = getchar();
        if(*s != c){
            printf("expect %c get %c\n", *s, c);
            return 0;
        }
        s++;
    }
    return 1;
}

int is_digit(char c){
    return c >= '0' && c <= '9'; 
}

int get_int(char *p){
    int n = 0, c;
    while(is_digit(c = getchar()))
        n = n * 10 + c - '0';
    if(p) *p = c;
    return n;
}
