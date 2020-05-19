#include "user_lib.h"
#include "my_string.h"

void printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    // add \r manually
    char new_fmt[1024];
    char* s = &fmt[0];
    char* p = &new_fmt[0];
    while (*s) {
        if (*s == '\n') *p++ = '\r';
        *p++ = *s++;
    }

    char buf[1024];
    unsigned int len = my_vsprintf(buf, new_fmt, args);
    uart_write(buf, len);
}
